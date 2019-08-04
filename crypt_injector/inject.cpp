#include "context.h"

#include <windows.h>
#include <stdio.h>
#include <stdint.h>
#include <tlhelp32.h>
#include <locale>
#include <ImageHlp.h>

class c_loader
{
public:
	struct data_t
	{
		data_t(uintptr_t _base, uintptr_t _entry_point)
		{
			base = _base;
			entry_point = _entry_point;
		}

		uintptr_t base;
		uintptr_t entry_point;
	};

	static int __stdcall loader_code(data_t* data, DWORD reason, void* reserved)
	{
		auto dll_main = reinterpret_cast<int(__stdcall*)(HMODULE, DWORD, void*)>(data->base + data->entry_point);

		//call entry of module
		dll_main((HMODULE)data->base, DLL_PROCESS_ATTACH, nullptr);
		return 1;
	}
};

// SHOUTS TO DEX
CLIENT_ID c_inject::find_process()
{
	CLIENT_ID pid = {};
	UNICODE_STRING sys_str;
	UNICODE_STRING sys_window_name{};
	sys_str.Length = ctx.m_window_class.length();
	sys_str.MaximumLength = 64;
	sys_str.Buffer = new wchar_t[64];

	mbstowcs(sys_str.Buffer, ctx.m_window_class.c_str(), ctx.m_window_class.length());

	HWND hwnd = do_syscall<HWND>(ctx.m_syscall.get_idx(fnvc("NtUserFindWindowEx")), nullptr, nullptr, &sys_str, &sys_window_name, 0);
	if (!hwnd)
		return pid;

	pid.UniqueProcess = do_syscall<HANDLE>(ctx.m_syscall.get_idx(fnvr("NtUserQueryWindow")), hwnd, 0);

	return pid;
}

uintptr_t c_inject::rva_to_offset(uintptr_t rva, PIMAGE_SECTION_HEADER section_raw, size_t count)
{
	size_t i = 0;
	for (i = 0; i < count; ++i)
	{
		auto section_begin_rva = section_raw[i].VirtualAddress;
		auto section_end_rva = section_begin_rva + section_raw[i].Misc.VirtualSize;
		if (section_begin_rva <= rva && rva <= section_end_rva)
			break;
	}

	return rva - section_raw[i].VirtualAddress + section_raw[i].PointerToRawData;
}

bool c_inject::fix_imports(uint8_t* dll)
{
	uint8_t* pRVA = nullptr;
	DWORD IAT_idx = 0;

	//get headers
	auto dos = (PIMAGE_DOS_HEADER)dll;
	auto nt = (PIMAGE_NT_HEADERS)(dll + dos->e_lfanew);
	auto section = (PIMAGE_SECTION_HEADER)(dll + dos->e_lfanew + sizeof(IMAGE_NT_HEADERS));

	auto sections_num = nt->FileHeader.NumberOfSections;
	auto import_table_rva = nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
	auto import_table_raw = rva_to_offset(import_table_rva, section, sections_num);
	auto import_table = (PIMAGE_IMPORT_DESCRIPTOR)(dll + import_table_raw);

	//resolve imports
	for (; import_table->Name; ++import_table)
	{
		auto lib_name = (LPCSTR)(dll + rva_to_offset(import_table->Name, section, sections_num));

		pRVA = reinterpret_cast<uint8_t*>(dll + rva_to_offset(import_table->OriginalFirstThunk ? import_table->OriginalFirstThunk : import_table->FirstThunk, section, sections_num));
		while (((const IMAGE_THUNK_DATA64*)(pRVA))->u1.AddressOfData)
		{
			uint64_t addr_of_data = ((const IMAGE_THUNK_DATA64*)(pRVA))->u1.AddressOfData;

			auto* addr_table = reinterpret_cast<IMAGE_IMPORT_BY_NAME*>(dll + rva_to_offset(addr_of_data, section, sections_num));
			if (addr_of_data < IMAGE_ORDINAL_FLAG64 && addr_table->Name[0])
			{
				uintptr_t func_addr = m_memory.proc_addr(lib_name, reinterpret_cast<const char*>(addr_table->Name));

				if (func_addr)
					((IMAGE_THUNK_DATA64*)(pRVA))->u1.Function = func_addr;
			}
			else
			{
				auto func_name = reinterpret_cast<const char*>(addr_of_data & 0xFFFF);
				uintptr_t func_addr = m_memory.proc_addr(lib_name, func_name);

				if (func_addr)
					((IMAGE_THUNK_DATA64*)(pRVA))->u1.Function = func_addr;
			}

			pRVA += sizeof(IMAGE_THUNK_DATA64);
			IAT_idx += sizeof(uint64_t);
		}
	}

	return true;
}

bool c_inject::rebase_image(uint8_t* dll, uintptr_t img_ptr)
{
	struct RelocData
	{
		ULONG PageRVA;
		ULONG BlockSize;

		struct
		{
			WORD Offset : 12;
			WORD Type : 4;
		}Item[1];
	};

	//get headers
	IMAGE_DOS_HEADER* dos_header = (PIMAGE_DOS_HEADER)dll;

	uintptr_t nt_header_addr = dos_header->e_lfanew;
	uintptr_t section_header_addr = dos_header->e_lfanew + sizeof(IMAGE_NT_HEADERS);

	IMAGE_NT_HEADERS* nt_header = (IMAGE_NT_HEADERS*)(&dll[nt_header_addr]);
	IMAGE_SECTION_HEADER* section_header = (IMAGE_SECTION_HEADER*)(&dll[section_header_addr]);

	//get data from headers
	uintptr_t image_base = nt_header->OptionalHeader.ImageBase;
	size_t    section_count = nt_header->FileHeader.NumberOfSections;
	uintptr_t reloc_section = nt_header->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress;

	uintptr_t delta = (uintptr_t)img_ptr - image_base;

	if (delta == 0)
		return true;

	if (!(nt_header->OptionalHeader.DllCharacteristics & IMAGE_DLLCHARACTERISTICS_DYNAMIC_BASE))
		return false;

	auto start = (uintptr_t)dll + rva_to_offset(reloc_section, section_header, section_count);
	auto end = start + nt_header->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].Size;
	auto fixrec = reinterpret_cast<RelocData*>(start);

	if (fixrec == nullptr)
		return true;

	while ((uintptr_t)fixrec < end&& fixrec->BlockSize)
	{
		uint32_t count = (fixrec->BlockSize - 8) >> 1;

		for (uint32_t i = 0; i < count; ++i)
		{
			uint16_t fixtype = (fixrec->Item[i].Type);
			uint16_t fixoffset = (fixrec->Item[i].Offset) % 4096;

			if (fixtype == IMAGE_REL_BASED_ABSOLUTE)
				continue;

			if (fixtype == IMAGE_REL_BASED_HIGHLOW || fixtype == IMAGE_REL_BASED_DIR64)
			{
				uintptr_t fixrva = (uintptr_t)fixrec->PageRVA + (uintptr_t)fixoffset;

				uint64_t val = *reinterpret_cast<uint64_t*>(dll + fixrva) + delta;
				*reinterpret_cast<uint64_t*>(dll + fixrva) = val;
			}
			else
				return false;
		}

		fixrec = reinterpret_cast<RelocData*>(reinterpret_cast<uintptr_t>(fixrec) + fixrec->BlockSize);
	}

	return true;
}

bool c_inject::inject_from_memory(uint8_t* dll)
{
	if (!m_memory.open(find_process()))
		return false;

	//get headers
	IMAGE_DOS_HEADER* dos_header = (PIMAGE_DOS_HEADER)dll;

	uintptr_t nt_header_addr = dos_header->e_lfanew;
	uintptr_t section_header_addr = dos_header->e_lfanew + sizeof(IMAGE_NT_HEADERS);

	IMAGE_NT_HEADERS* nt_header = (IMAGE_NT_HEADERS*)(&dll[nt_header_addr]);
	IMAGE_SECTION_HEADER* section_header = (IMAGE_SECTION_HEADER*)(&dll[section_header_addr]);

	//get data from headers
	size_t    size_of_image = nt_header->OptionalHeader.SizeOfImage;
	size_t    size_of_headers = nt_header->OptionalHeader.SizeOfHeaders;
	uintptr_t image_base = nt_header->OptionalHeader.ImageBase;
	uintptr_t entry_point = nt_header->OptionalHeader.AddressOfEntryPoint;

	size_t    section_count = nt_header->FileHeader.NumberOfSections;
	uintptr_t reloc_section = nt_header->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress;
	uintptr_t import_section = nt_header->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;

	if (!fix_imports(dll))
		return false;

	void* module_address = (void*)((uintptr_t)dll + rva_to_offset(image_base, section_header, section_count));

	//Allocate image
	if (!m_memory.alloc(module_address, &size_of_image, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE))
	{
		module_address = nullptr;
		if (!m_memory.alloc(module_address, &size_of_image, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE))
			return false;
	}

	if (!rebase_image(dll, (uintptr_t)module_address))
		return false;

	if (!m_memory.write(module_address, dll, size_of_headers))
		return false;

	//write sections
	for (size_t i = 0; i < section_count; i++)
	{
		uintptr_t virtual_address = section_header[i].VirtualAddress + uintptr_t(module_address);
		uintptr_t raw_data_address = section_header[i].PointerToRawData;
		uintptr_t raw_data_size = section_header[i].SizeOfRawData;

		if (!m_memory.write((void*)virtual_address, &dll[raw_data_address], raw_data_size))
			return false;
	}

	c_loader::data_t loader_data((uintptr_t)module_address, entry_point);

	void* loader_data_addr = nullptr;
	void* loader_code_addr = nullptr;

	size_t data_size = sizeof c_loader::data_t;
	if (!m_memory.alloc(loader_data_addr, &data_size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE))
		return false;

	size_t code_size = 0x1000;
	if (!m_memory.alloc(loader_code_addr, &code_size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE))
		return false;

	if (!m_memory.write(loader_data_addr, &loader_data, data_size))
		return false;

	if (!m_memory.write(loader_code_addr, &c_loader::loader_code, code_size))
		return;
	
	HANDLE thread = nullptr;
	if (!m_memory.thread(thread, (void*)loader_code_addr, loader_data_addr));
		return false;

	LI_FN(CloseHandle).cached()(thread);

	return true;
}
