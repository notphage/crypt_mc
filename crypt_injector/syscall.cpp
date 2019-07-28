#include "context.h"

uint8_t* c_syscall::load_ntdll()
{
	// we load ntdll from disk, as u cant edit this ( causes bsod unless patchguard is disabled )
	// could even compare ntdll in process to disk, to find potentially malicious users

	char path[MAX_PATH];
	GetSystemDirectoryA(path, MAX_PATH);

	std::string ntdll_path(path);
	ntdll_path += "\\ntdll.dll";

	FILE* file;
	if (fopen_s(&file, ntdll_path.c_str(), "rb") != 0)
		return nullptr;

	fseek(file, 0, SEEK_END);
	size_t ntdll_size = ftell(file);
	rewind(file);

	uint8_t* ntdll = new uint8_t[ntdll_size];
	fread(ntdll, ntdll_size, 1, file);
	fclose(file);

	return ntdll;
}

bool c_syscall::init()
{
	uint8_t* ntdll = load_ntdll();
	if (!ntdll)
		return false;

	IMAGE_DOS_HEADER* dos_header = (IMAGE_DOS_HEADER*)(&ntdll[0]);
	IMAGE_NT_HEADERS* nt_header = (IMAGE_NT_HEADERS*)(&ntdll[dos_header->e_lfanew]);

	if (dos_header->e_magic != IMAGE_DOS_SIGNATURE)
	{
		delete[] ntdll;
		return false;
	}

	if (nt_header->Signature != IMAGE_NT_SIGNATURE)
	{
		delete[] ntdll;
		return false;
	}

	IMAGE_SECTION_HEADER* section_header = (IMAGE_SECTION_HEADER*)(&ntdll[dos_header->e_lfanew + sizeof(IMAGE_NT_HEADERS)]);
	uintptr_t export_rva = nt_header->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;

	uint32_t delta = 0;
	for (size_t i = 0; i < nt_header->FileHeader.NumberOfSections; i++)
	{
		if (export_rva > section_header[i].VirtualAddress)
			delta = section_header[i].VirtualAddress - section_header[i].PointerToRawData;
	}

	// walk export directory
	IMAGE_EXPORT_DIRECTORY* export_directory = (IMAGE_EXPORT_DIRECTORY*)(&ntdll[export_rva - delta]);

	size_t number_of_functions = export_directory->NumberOfFunctions;
	uintptr_t names = export_directory->AddressOfNames - delta;
	uintptr_t funcs = export_directory->AddressOfFunctions - delta;
	uintptr_t ords = export_directory->AddressOfNameOrdinals - delta;

	for (size_t i = 0; i < number_of_functions; i++)
	{
		uint32_t name_rva = *(uint32_t*)(&ntdll[names + i * sizeof(uint32_t)]) - delta;
		char* name = (char*)(&ntdll[name_rva]);

		uint16_t ordinal = *(uint16_t*)(&ntdll[ords + i * sizeof(uint16_t)]);
		uint32_t func_rva = *(uint32_t*)(&ntdll[funcs + ordinal * sizeof(uint32_t)]);

		uint32_t func_delta = 0;
		for (size_t j = 0; j < nt_header->FileHeader.NumberOfSections; j++)
		{
			if (func_rva > section_header[j].VirtualAddress)
				func_delta = section_header[j].VirtualAddress - section_header[j].PointerToRawData;
		}

		func_rva -= func_delta;

		uint32_t code = *(uint32_t*)(&ntdll[func_rva + 0]);
		uint32_t index = *(uint32_t*)(&ntdll[func_rva + 4]);

		// we got a syscall, time to get the index
		if (code == 0xB8D18B4C)
			syscalls.emplace(fnvr(name), index);
	}

	delete[] ntdll;
	return true;
}