#include "context.h"

uint8_t* c_syscall::load_dll(const std::string& name)
{
	// we load ntdll from disk, as u cant edit this ( causes bsod unless patchguard is disabled )
	// could even compare ntdll in process to disk, to find potentially malicious users

	char path[MAX_PATH]{};
	LI_FN(GetSystemDirectoryA).cached()(path, MAX_PATH);

	std::string dll_path(path);
	dll_path += name;

	FILE* file = nullptr;
	if (LI_FN(fopen_s).cached()(&file, dll_path.c_str(), xors("rb")) != 0)
		return nullptr;

	LI_FN(fseek).cached()(file, 0, SEEK_END);
	size_t dll_size = LI_FN(ftell).cached()(file);
	LI_FN(rewind).cached()(file);

	uint8_t* dll = new uint8_t[dll_size];
	LI_FN(fread).cached()(dll, dll_size, 1, file);
	LI_FN(fclose).cached()(file);

	return dll;
}

bool c_syscall::scan_dll(uint8_t* dll)
{
	IMAGE_DOS_HEADER* dos_header = (IMAGE_DOS_HEADER*)(&dll[0]);
	IMAGE_NT_HEADERS* nt_header = (IMAGE_NT_HEADERS*)(&dll[dos_header->e_lfanew]);

	if (dos_header->e_magic != IMAGE_DOS_SIGNATURE)
	{
		delete[] dll;
		return false;
	}

	if (nt_header->Signature != IMAGE_NT_SIGNATURE)
	{
		delete[] dll;
		return false;
	}

	IMAGE_SECTION_HEADER* section_header = (IMAGE_SECTION_HEADER*)(&dll[dos_header->e_lfanew + sizeof(IMAGE_NT_HEADERS)]);
	uintptr_t export_rva = nt_header->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;

	uint32_t delta = 0;
	for (size_t i = 0; i < nt_header->FileHeader.NumberOfSections; i++)
	{
		if (export_rva > section_header[i].VirtualAddress)
			delta = section_header[i].VirtualAddress - section_header[i].PointerToRawData;
	}

	// walk export directory
	IMAGE_EXPORT_DIRECTORY* export_directory = (IMAGE_EXPORT_DIRECTORY*)(&dll[export_rva - delta]);

	size_t number_of_functions = export_directory->NumberOfFunctions;
	uintptr_t names = export_directory->AddressOfNames - delta;
	uintptr_t funcs = export_directory->AddressOfFunctions - delta;
	uintptr_t ords = export_directory->AddressOfNameOrdinals - delta;

	for (size_t i = 0; i < number_of_functions; i++)
	{
		uint32_t name_rva = *(uint32_t*)(&dll[names + i * sizeof(uint32_t)]) - delta;
		char* name = (char*)(&dll[name_rva]);

		uint16_t ordinal = *(uint16_t*)(&dll[ords + i * sizeof(uint16_t)]);
		uint32_t func_rva = *(uint32_t*)(&dll[funcs + ordinal * sizeof(uint32_t)]);

		uint32_t func_delta = 0;
		for (size_t j = 0; j < nt_header->FileHeader.NumberOfSections; j++)
		{
			if (func_rva > section_header[j].VirtualAddress)
				func_delta = section_header[j].VirtualAddress - section_header[j].PointerToRawData;
		}

		func_rva -= func_delta;

		uint32_t code = *(uint32_t*)(&dll[func_rva + 0]);
		uint32_t index = *(uint32_t*)(&dll[func_rva + 4]);

		// we got a syscall, time to get the index
		if (code == 0xB8D18B4C)
			syscalls.emplace(fnvr(name), index);
	}

	delete[] dll;
	return true;
}

bool c_syscall::init()
{
	uint8_t* ntdll = load_dll("\\ntdll.dll");
	if (!ntdll)
		return false;

	if (!scan_dll(ntdll))
		return false;

	uint8_t* win32u = load_dll("\\win32u.dll");
	if (!win32u)
		return false;

	if (!scan_dll(win32u))
		return false;

	return true;
}