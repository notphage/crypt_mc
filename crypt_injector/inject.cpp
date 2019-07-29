#include "context.h"

#include <windows.h>
#include <stdio.h>
#include <stdint.h>
#include <tlhelp32.h>
#include <ntstatus.h>
#include <locale>

class c_loader
{
public:
	struct data_t
	{
		data_t(uintptr_t _base, uintptr_t _image_base, uintptr_t _entry_point, uintptr_t _base_relocation, uintptr_t _import_directory, uintptr_t _loadlib, uintptr_t _get_proc_address, uintptr_t _messagebox_func)
		{
			base = _base;
			image_base = _image_base;
			entry_point = _entry_point;
			base_relocation = _base_relocation;
			import_directory = _import_directory;
			loadlib = _loadlib;
			get_proc_address = _get_proc_address;
			messagebox_func = _messagebox_func;
		}

		uintptr_t base;
		uintptr_t image_base;
		uintptr_t entry_point;
		uintptr_t base_relocation;
		uintptr_t import_directory;
		uintptr_t loadlib;
		uintptr_t get_proc_address;
		uintptr_t messagebox_func;
	};

	static int __stdcall loader_code(data_t* data, DWORD reason, void* reserved)
	{
		uintptr_t base = data->base;
		uintptr_t delta = base - data->image_base;

		IMAGE_BASE_RELOCATION* base_relocation = (IMAGE_BASE_RELOCATION*)(base + data->base_relocation);
		IMAGE_IMPORT_DESCRIPTOR* import_directory = (IMAGE_IMPORT_DESCRIPTOR*)(base + data->import_directory);

		auto dll_main = reinterpret_cast<int(__stdcall*)(HMODULE, DWORD, void*)>(base + data->entry_point);
		auto loadlib = reinterpret_cast<HMODULE(__stdcall*)(LPCSTR)>(data->loadlib);
		auto get_proc_address = reinterpret_cast<FARPROC(__stdcall*)(HMODULE, LPCSTR)>(data->get_proc_address);
		auto msgbox = reinterpret_cast<int(WINAPI*)(HWND, LPCSTR, LPCSTR, UINT)>(data->messagebox_func);

		//relocate the image
		while (base_relocation->SizeOfBlock > 0)
		{
			uintptr_t start_address = base + base_relocation->VirtualAddress;

			//relocations in this block
			size_t reloc_count = (base_relocation->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION)) / sizeof(uint16_t);

			for (size_t i = 0; i < reloc_count; i++)
			{
				uint16_t reloc_data = *(uint16_t*)(uintptr_t(base_relocation) + sizeof(IMAGE_BASE_RELOCATION) + sizeof(uint16_t) * i);
				uint16_t reloc_type = reloc_data & 0xF000;
				uint16_t reloc_offset = reloc_data & 0x0FFF;

				//this could be wrong
				if (reloc_type == 0xA000)
					* (uintptr_t*)(start_address + reloc_offset) += delta;
			}

			base_relocation = (IMAGE_BASE_RELOCATION*)(uintptr_t(base_relocation) + base_relocation->SizeOfBlock);
		}

		//resolve imports
		while (import_directory->Characteristics)
		{
			//get thunk data
			IMAGE_THUNK_DATA* original_first_thunk = (IMAGE_THUNK_DATA*)(base + import_directory->OriginalFirstThunk);
			IMAGE_THUNK_DATA* first_thunk = (IMAGE_THUNK_DATA*)(base + import_directory->FirstThunk);

			//load the requested module with loadlibrary :>
			HMODULE import_module = loadlib((LPCSTR)(base + import_directory->Name));

			if (!import_module)
				msgbox(nullptr, (LPCSTR)(base + import_directory->Name), xors("LIB"), MB_OK);

			//bb got dat func in da thunk
			while (original_first_thunk->u1.AddressOfData)
			{
				if (original_first_thunk->u1.Ordinal & IMAGE_ORDINAL_FLAG)
				{
					//resolve import func by ordinal (REALLY UNCOMMON)
					LPCSTR func_name = (LPCSTR)(original_first_thunk->u1.Ordinal & 0xFFFF);
					uintptr_t func_addr = (uintptr_t)get_proc_address(import_module, func_name);

					if (!func_addr)
						msgbox(nullptr, (LPCSTR)(base + import_directory->Name), xors("ORD"), MB_OK);

					first_thunk->u1.Function = func_addr;
				}
				else
				{
					//resolve import func by name
					IMAGE_IMPORT_BY_NAME* import_by_name = (IMAGE_IMPORT_BY_NAME*)(base + original_first_thunk->u1.AddressOfData);
					LPCSTR func_name = (LPCSTR)import_by_name->Name;
					uintptr_t func_addr = (uintptr_t)get_proc_address(import_module, func_name);

					if (!func_addr)
						msgbox(nullptr, (LPCSTR)(base + import_directory->Name), xors("IBM"), MB_OK);

					first_thunk->u1.Function = func_addr;
				}

				original_first_thunk++;
				first_thunk++;
			}

			import_directory++;
		}

		//call entry of module
		dll_main((HMODULE)base, DLL_PROCESS_ATTACH, nullptr);
		return 1;
	}
};

CLIENT_ID c_inject::find_process()
{
	CLIENT_ID pid = {};
	GetWindowThreadProcessId(FindWindowA(ctx.m_window_class.c_str(), nullptr), (PDWORD)&pid.UniqueProcess);

	return pid;
}

bool c_inject::inject_from_memory(uint8_t* dll)
{
	HANDLE process = {};

	// Process Handle
	{
		auto proc_id = find_process();
		static OBJECT_ATTRIBUTES zoa = { sizeof(zoa) };
		if (auto status = do_syscall<NTSTATUS>(ctx.m_syscall.get_idx(fnvc("NtOpenProcess")), &process, PROCESS_ALL_ACCESS, &zoa, &proc_id); status != STATUS_SUCCESS)
			return false;

		if (!process)
			return false;
	}

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

	void* module_address = nullptr;

	//Allocate image
	{
		if (auto status = do_syscall<NTSTATUS>(ctx.m_syscall.get_idx(fnvc("NtAllocateVirtualMemory")), process, &module_address, 0, &size_of_image, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE); status != STATUS_SUCCESS)
			return false;

		if (!module_address)
			return false;
	}

	if (auto status = do_syscall<NTSTATUS>(ctx.m_syscall.get_idx(fnvc("NtWriteVirtualMemory")), process, module_address, dll, size_of_headers, nullptr); status != STATUS_SUCCESS)
		return false;

	//write sections
	for (size_t i = 0; i < section_count; i++)
	{
		uintptr_t virtual_address = section_header[i].VirtualAddress + uintptr_t(module_address);
		uintptr_t raw_data_address = section_header[i].PointerToRawData;
		uintptr_t raw_data_size = section_header[i].SizeOfRawData;

		if (auto status = do_syscall<NTSTATUS>(ctx.m_syscall.get_idx(fnvc("NtWriteVirtualMemory")), process, (void*)virtual_address, &dll[raw_data_address], raw_data_size, nullptr); status != STATUS_SUCCESS)
			return false;
	}

	//do da loada
	c_loader::data_t loader_data((uintptr_t)module_address,
		image_base,
		entry_point,
		reloc_section,
		import_section,
		(uintptr_t)LoadLibraryA,
		(uintptr_t)GetProcAddress,
		(uintptr_t)MessageBoxA
	);

	void* loader_data_address = nullptr;
	void* loader_code_address = nullptr;

	//Allocate code and data
	{
		uint64_t data_size = sizeof(c_loader::data_t);
		if (auto status = do_syscall<NTSTATUS>(ctx.m_syscall.get_idx(fnvc("NtAllocateVirtualMemory")), process, &loader_data_address, 0, &data_size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE); status != STATUS_SUCCESS)
			return false;

		if (!loader_data_address)
			return false;

		uint64_t code_size = 0x4000;
		if (auto status = do_syscall<NTSTATUS>(ctx.m_syscall.get_idx(fnvc("NtAllocateVirtualMemory")), process, &loader_code_address, 0, &code_size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE); status != STATUS_SUCCESS)
			return false;

		if (!loader_data_address)
			return false;
	}

	if (auto status = do_syscall<NTSTATUS>(ctx.m_syscall.get_idx(fnvc("NtWriteVirtualMemory")), process, loader_data_address, &loader_data, sizeof(c_loader::data_t), nullptr); status != STATUS_SUCCESS)
		return false;

	if (auto status = do_syscall<NTSTATUS>(ctx.m_syscall.get_idx(fnvc("NtWriteVirtualMemory")), process, loader_code_address, c_loader::loader_code, 0x4000, nullptr); status != STATUS_SUCCESS)
		return false;

	HANDLE thread = nullptr;
	if (auto status = do_syscall<NTSTATUS>(ctx.m_syscall.get_idx(fnvc("NtCreateThreadEx")), &thread, 0x1FFFFF, 0, process, reinterpret_cast<LPTHREAD_START_ROUTINE>(loader_code_address), loader_data_address, 0, nullptr, nullptr, nullptr, nullptr); status != STATUS_SUCCESS)
		return false;

	if (!thread)
		return false;

	CloseHandle(thread);
	CloseHandle(process);

	return true;
}
