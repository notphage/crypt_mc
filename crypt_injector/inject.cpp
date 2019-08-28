#include "context.h"

#include <windows.h>
#include <stdio.h>
#include <stdint.h>
#include <tlhelp32.h>
#include <locale>

uint8_t load_shellcode[] = {
	0x51, // push rcx
	0x52, // push rdx
	0x55, // push rbp
	0x56, // push rsi
	0x53, // push rbx
	0x57, // push rdi
	0x41, 0x50, // push r8
	0x41, 0x51, // push r9
	0x41, 0x52, // push r10
	0x41, 0x53, // push r11
	0x41, 0x54, // push r12
	0x41, 0x55, // push r13
	0x41, 0x56, // push r14
	0x41, 0x57, // push r15
	0x48, 0xB8, 0xFF, 0x00, 0xDE, 0xAD, 0xBE, 0xEF, 0x00, 0xFF, // mov rax,0xff00efbeadde00ff
	0x48, 0xBA, 0xFF, 0x00, 0xDE, 0xAD, 0xC0, 0xDE, 0x00, 0xFF, // mov rdx,0xff00dec0adde00ff
	0x48, 0x89, 0x10, // mov [rax], rdx
	0x48, 0x31, 0xC0, // xor rax, rax
	0x48, 0x31, 0xD2, // xor rdx, rdx
	0x48, 0x83, 0xEC, 0x28, // sub rsp, 0x28
	0x48, 0xB9, 0xDE, 0xAD, 0xBE, 0xEF, 0xDE, 0xAD, 0xBE, 0xEF, // mov rcx, 0xefbeaddeefbeadde
	0x48, 0x31, 0xD2, // xor rdx, rdx
	0x48, 0x83, 0xC2, 0x01, // add rdx, 0x1 (DLL_PROCESS_ATTACH)
	0x48, 0xB8, 0xDE, 0xAD, 0xC0, 0xDE, 0xDE, 0xAD, 0xC0, 0xDE, // mov rax, 0xdec0addedec0adde
	0xFF, 0xD0, // cal rax
	0x48, 0x83, 0xC4, 0x28, // add rsp, 0x28
	0x41, 0x5F, // pop r15
	0x41, 0x5E, // pop r14
	0x41, 0x5D, // pop r13
	0x41, 0x5C, // pop r12
	0x41, 0x5B, // pop r11
	0x41, 0x5A, // pop r10
	0x41, 0x59, // pop r9
	0x41, 0x58, // pop r8
	0x5F, // pop rdi
	0x5B, // pop rbx
	0x5E, // pop rsi
	0x5D, // pop rbp
	0x5A, // pop rdx
	0x59, // pop rcx
	0x48, 0x31, 0xC0, // xor rax, rax
	0xC3 // ret
};

uint8_t unload_shellcode[] = {
	0x51, // push rcx
	0x52, // push rdx
	0x55, // push rbp
	0x56, // push rsi
	0x53, // push rbx
	0x57, // push rdi
	0x41, 0x50, // push r8
	0x41, 0x51, // push r9
	0x41, 0x52, // push r10
	0x41, 0x53, // push r11
	0x41, 0x54, // push r12
	0x41, 0x55, // push r13
	0x41, 0x56, // push r14
	0x41, 0x57, // push r15
	0x48, 0xB8, 0xFF, 0x00, 0xDE, 0xAD, 0xBE, 0xEF, 0x00, 0xFF, // mov rax,0xff00efbeadde00ff
	0x48, 0xBA, 0xFF, 0x00, 0xDE, 0xAD, 0xC0, 0xDE, 0x00, 0xFF, // mov rdx,0xff00dec0adde00ff
	0x48, 0x89, 0x10, // mov [rax], rdx
	0x48, 0x31, 0xC0, // xor rax, rax
	0x48, 0x31, 0xD2, // xor rdx, rdx
	0x48, 0x83, 0xEC, 0x28, // sub rsp, 0x28
	0x48, 0xB9, 0xDE, 0xAD, 0xBE, 0xEF, 0xDE, 0xAD, 0xBE, 0xEF, // mov rcx, 0xefbeaddeefbeadde
	0x48, 0x31, 0xD2, // xor rdx, rdx
	0x48, 0xB8, 0xDE, 0xAD, 0xC0, 0xDE, 0xDE, 0xAD, 0xC0, 0xDE, // mov rax, 0xdec0addedec0adde
	0xFF, 0xD0, // cal rax
	0x48, 0x83, 0xC4, 0x28, // add rsp, 0x28
	0x41, 0x5F, // pop r15
	0x41, 0x5E, // pop r14
	0x41, 0x5D, // pop r13
	0x41, 0x5C, // pop r12
	0x41, 0x5B, // pop r11
	0x41, 0x5A, // pop r10
	0x41, 0x59, // pop r9
	0x41, 0x58, // pop r8
	0x5F, // pop rdi
	0x5B, // pop rbx
	0x5E, // pop rsi
	0x5D, // pop rbp
	0x5A, // pop rdx
	0x59, // pop rcx
	0x48, 0x31, 0xC0, // xor rax, rax
	0xC3 // ret
};

// SHOUTS TO DEX
CLIENT_ID c_inject::find_process()
{
	CLIENT_ID pid = {};
	UNICODE_STRING sys_str;
	UNICODE_STRING sys_window_name{};
	sys_str.Length = static_cast<uint16_t>(ctx.m_window_class.length());
	sys_str.MaximumLength = 64;
	sys_str.Buffer = new wchar_t[64];

	mbstowcs(sys_str.Buffer, ctx.m_window_class.c_str(), ctx.m_window_class.length());

	const HWND hwnd = do_syscall<HWND>(ctx.m_syscall.get_idx(fnvc("NtUserFindWindowEx")), nullptr, nullptr, &sys_str, &sys_window_name, 0);
	if (!hwnd)
		return pid;

	pid.UniqueProcess = do_syscall<HANDLE>(ctx.m_syscall.get_idx(fnvr("NtUserQueryWindow")), hwnd, 0);

	return pid;
}

uint64_t* c_inject::get_ptr_from_rva(uint64_t rva, IMAGE_NT_HEADERS* nt_header, uint8_t* image_base)
{
	const PIMAGE_SECTION_HEADER section_header{ get_enclosing_section_header(rva, nt_header) };

	if (!section_header)
		return nullptr;

	const int64_t delta{ static_cast<int64_t>(section_header->VirtualAddress - section_header->PointerToRawData) };
	return reinterpret_cast<uint64_t*>(image_base + rva - delta);
}

PIMAGE_SECTION_HEADER c_inject::get_enclosing_section_header(uint64_t rva, PIMAGE_NT_HEADERS nt_header)
{
	PIMAGE_SECTION_HEADER section{ IMAGE_FIRST_SECTION(nt_header) };

	for (int i = 0; i < nt_header->FileHeader.NumberOfSections; i++, section++)
	{
		uint64_t size{ section->Misc.VirtualSize };
		if (!size)
			size = section->SizeOfRawData;

		if ((rva >= section->VirtualAddress) && (rva < (section->VirtualAddress + size)))
			return section;
	}

	return nullptr;
}

void c_inject::solve_imports(uint8_t* base, IMAGE_NT_HEADERS* nt_header, IMAGE_IMPORT_DESCRIPTOR* import_descriptor)
{
	char* module;
	while ((module = reinterpret_cast<char*>(get_ptr_from_rva(static_cast<uint64_t>(import_descriptor->Name), nt_header, base))))
	{
		auto* thunk_data{ reinterpret_cast<IMAGE_THUNK_DATA*>(get_ptr_from_rva(static_cast<uint64_t>(import_descriptor->FirstThunk), nt_header, base)) };

		while (thunk_data->u1.AddressOfData)
		{
			auto* iibn{ reinterpret_cast<IMAGE_IMPORT_BY_NAME*>(get_ptr_from_rva(static_cast<uint64_t>(thunk_data->u1.AddressOfData), nt_header, base)) };
			thunk_data->u1.Function = static_cast<uint64_t>(m_memory.proc_addr(module, static_cast<char*>(iibn->Name)));
			thunk_data++;
		}
		
		import_descriptor++;
	}
}

void c_inject::solve_relocations(uint64_t base, uint64_t relocation_base, IMAGE_NT_HEADERS* nt_header, IMAGE_BASE_RELOCATION* reloc, size_t size)
{
	const uint64_t image_base{ nt_header->OptionalHeader.ImageBase };
	const uint64_t delta{ relocation_base - image_base };
	unsigned int bytes{ 0 };

	while (bytes < size) {
		auto* reloc_base{ static_cast<uint64_t*>(get_ptr_from_rva(static_cast<uint64_t>(reloc->VirtualAddress), nt_header, reinterpret_cast<uint8_t*>(base))) };
		const auto num_of_relocations{ (reloc->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION)) / sizeof(WORD) };
		auto reloc_data = reinterpret_cast<uint16_t*>(reinterpret_cast<uint64_t>(reloc) + sizeof(IMAGE_BASE_RELOCATION));

		for (unsigned int i = 0; i < num_of_relocations; i++)
		{
			if (((*reloc_data >> 12) & IMAGE_REL_BASED_HIGHLOW))
				*reinterpret_cast<uint64_t*>(reinterpret_cast<uint64_t>(reloc_base) + static_cast<uint64_t>(*reloc_data & 0x0FFF)) += delta;
			
			reloc_data++;
		}

		bytes += reloc->SizeOfBlock;
		reloc = reinterpret_cast<IMAGE_BASE_RELOCATION*>(reloc_data);
	}
}

void c_inject::map_pe_sections(uint64_t base, IMAGE_NT_HEADERS* nt_header)
{
	auto header{ IMAGE_FIRST_SECTION(nt_header) };
	size_t virtual_size{ 0 };
	size_t bytes{ 0 };

	while (nt_header->FileHeader.NumberOfSections && bytes < nt_header->OptionalHeader.SizeOfImage)
	{
		m_memory.write(reinterpret_cast<void*>(base + header->VirtualAddress), reinterpret_cast<uintptr_t>(m_raw_data + header->PointerToRawData), header->SizeOfRawData);
		virtual_size = header->VirtualAddress;
		virtual_size = (++header)->VirtualAddress - virtual_size;
		bytes += virtual_size;

		// TODO Set proper page protection
	}
}

bool c_inject::parse_imports()
{
	const auto base{ m_memory.get_base() };

	IMAGE_DOS_HEADER dos_header{ };
	if (!m_memory.read(base, static_cast<void*>(&dos_header), sizeof IMAGE_DOS_HEADER))
		return false;
	
	IMAGE_NT_HEADERS nt_headers{ };
	if (!m_memory.read(reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(base) + dos_header.e_lfanew), static_cast<void*>(&nt_headers), sizeof IMAGE_NT_HEADERS))
		return false;
	
	IMAGE_IMPORT_DESCRIPTOR descriptor{ };
	if (!m_memory.read(reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(base) + nt_headers.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress), static_cast<void*>(&descriptor), sizeof IMAGE_IMPORT_DESCRIPTOR))
		return false;

	int descriptor_count{ 0 };

	while (descriptor.Characteristics)
	{
		IMAGE_THUNK_DATA first_thunk{ };
		if (!m_memory.read(reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(base) + descriptor.FirstThunk), static_cast<void*>(&first_thunk), sizeof IMAGE_THUNK_DATA))
			return false;
		
		IMAGE_THUNK_DATA original_first_thunk{ };
		if (!m_memory.read(reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(base) + descriptor.OriginalFirstThunk), static_cast<void*>(&original_first_thunk), sizeof IMAGE_THUNK_DATA))
			return false;
		
		int thunk_count = 0;

		while (original_first_thunk.u1.AddressOfData)
		{
			char name[256];
			if (!m_memory.read(reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(base) + original_first_thunk.u1.AddressOfData + 0x2), static_cast<void*>(&name), sizeof name))
				return false;
			
			std::string str_name(name);
			const auto thunk_offset{ thunk_count * sizeof(uintptr_t) };

			if (str_name.length() > 0)
				m_imports[fnvr(str_name.c_str())] = reinterpret_cast<uintptr_t>(base) + descriptor.FirstThunk + thunk_offset;

			++thunk_count;
			if (!m_memory.read(reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(base) + descriptor.FirstThunk + sizeof(IMAGE_THUNK_DATA) * thunk_count), static_cast<void*>(&first_thunk), sizeof IMAGE_THUNK_DATA))
				return false;
			
			if (!m_memory.read(reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(base) + descriptor.OriginalFirstThunk + sizeof(IMAGE_THUNK_DATA) * thunk_count), static_cast<void*>(&original_first_thunk), sizeof IMAGE_THUNK_DATA))
				return false;
		}

		++descriptor_count;
		if (!m_memory.read(reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(base) + nt_headers.OptionalHeader.DataDirectory[1].VirtualAddress + sizeof(IMAGE_IMPORT_DESCRIPTOR) * descriptor_count), static_cast<void*>(&descriptor), sizeof IMAGE_IMPORT_DESCRIPTOR))
			return false;
	}

	return !m_imports.empty();
}

bool c_inject::inject_from_memory(uint8_t* dll)
{
	m_raw_data = dll;
	if (!m_memory.open(find_process()))
		return false;

	//get headers
	auto* dos_header{ reinterpret_cast<IMAGE_DOS_HEADER*>(m_raw_data) };
	auto* nt_header{ reinterpret_cast<IMAGE_NT_HEADERS*>(&m_raw_data[dos_header->e_lfanew]) };

	m_cheat_size = nt_header->OptionalHeader.SizeOfImage;
	if (!m_memory.alloc(m_cheat_base, &m_cheat_size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE))
		return false;

	m_loader_size = sizeof load_shellcode;
	if (!m_memory.alloc(m_loader_stub, &m_loader_size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE))
		return false;

	auto* import_descriptor{ reinterpret_cast<IMAGE_IMPORT_DESCRIPTOR*>(get_ptr_from_rva(static_cast<uint64_t>(nt_header->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress), nt_header, m_raw_data)) };

	if (nt_header->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].Size)
		solve_imports(m_raw_data, nt_header, import_descriptor);

	auto* base_reloc{ reinterpret_cast<IMAGE_BASE_RELOCATION*>(get_ptr_from_rva(nt_header->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress, nt_header, m_raw_data)) };
	if (nt_header->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].Size)
		solve_relocations(reinterpret_cast<uint64_t>(m_raw_data), reinterpret_cast<uint64_t>(m_cheat_base), nt_header, base_reloc, nt_header->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC].Size);

	if (!parse_imports())
		return false;

	void* iat_func{ reinterpret_cast<void*>(m_imports[(fnvc("LoadLibraryA"))]) };
	if (!iat_func)
		return false;

	uintptr_t original_iat_func = 0;
	if (!m_memory.read(iat_func, &original_iat_func, sizeof original_iat_func))
		return false;

	*reinterpret_cast<uintptr_t*>(load_shellcode + 0x18) = reinterpret_cast<uintptr_t>(iat_func);
	*reinterpret_cast<uintptr_t*>(load_shellcode + 0x22) = original_iat_func;
	/* Save pointer and orginal function address for stub to restore it.
		mov rax, 0xff00efbeadde00ff  ; dll_stub + 0x18 (iat_function_ptr)
		mov rdx, 0xff00dec0adde00ff  ; dll_stub + 0x22 (orginal_function_addr)
		mov qword [rax], rdx
		xor rax, rax
		xor rdx, rdx
	*/

	if (!m_memory.write(m_cheat_base, m_raw_data, nt_header->FileHeader.SizeOfOptionalHeader + sizeof nt_header->FileHeader + sizeof nt_header->Signature))
		return false;

	map_pe_sections(reinterpret_cast<uintptr_t>(m_cheat_base), nt_header);

	const uint64_t entry_point{ reinterpret_cast<uint64_t>(m_cheat_base) + nt_header->OptionalHeader.AddressOfEntryPoint };
	*reinterpret_cast<uint64_t*>(load_shellcode + 0x39) = reinterpret_cast<uint64_t>(m_cheat_base);
	*reinterpret_cast<uint64_t*>(load_shellcode + 0x4a) = entry_point;
	/* Save module_base and entry_point to call dllmain correctly
	sub rsp, 0x28
	mov rcx, 0xefbeaddeefbeadde ; dll_stub + 0x39 (base)
	xor rdx, rdx
	add rdx, 1
	mov rax, 0xdec0addedec0adde ; dll_stub + 0x4a (entry_point)
	call rax
	*/

	if (!m_memory.write(m_loader_stub, load_shellcode, m_loader_size))
		return false;

	int32_t old_protect = 0;
	size_t iat_size = sizeof uint64_t;
	if (!m_memory.protect(iat_func, &iat_size, PAGE_READWRITE, old_protect))
		return false;

	if (!m_memory.write(iat_func, reinterpret_cast<uintptr_t>(&m_loader_stub), sizeof uint64_t))
		return false;

	if (!m_memory.protect(iat_func, &iat_size, old_protect, old_protect))
		return false;
	
	return true;
}

bool c_inject::unload()
{
	////do da loada
	//c_unloader::data_t loader_data((uintptr_t)m_cheat_addr,
	//	m_entry_point
	//);
	//
	////Allocate code and data
	//{
	//	m_unloader_data_size = sizeof(c_unloader::data_t);
	//	if (auto status = do_syscall<NTSTATUS>(ctx.m_syscall.get_idx(fnvc("NtAllocateVirtualMemory")), m_proc, &m_unloader_data_addr, 0, &m_unloader_data_size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE); status != STATUS_SUCCESS)
	//		return false;
	//
	//	if (!m_unloader_data_addr)
	//		return false;
	//
	//	m_unloader_code_size = 0x4000;
	//	if (auto status = do_syscall<NTSTATUS>(ctx.m_syscall.get_idx(fnvc("NtAllocateVirtualMemory")), m_proc, &m_unloader_code_addr, 0, &m_unloader_code_size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE); status != STATUS_SUCCESS)
	//		return false;
	//
	//	if (!m_unloader_code_addr)
	//		return false;
	//}
	//
	//if (auto status = do_syscall<NTSTATUS>(ctx.m_syscall.get_idx(fnvc("NtWriteVirtualMemory")), m_proc, m_unloader_data_addr, &loader_data, sizeof(c_unloader::data_t), nullptr); status != STATUS_SUCCESS)
	//	return false;
	//
	//if (auto status = do_syscall<NTSTATUS>(ctx.m_syscall.get_idx(fnvc("NtWriteVirtualMemory")), m_proc, m_unloader_code_addr, c_unloader::loader_code, 0x4000, nullptr); status != STATUS_SUCCESS)
	//	return false;
	//
	//HANDLE thread = nullptr;
	//if (auto status = do_syscall<NTSTATUS>(ctx.m_syscall.get_idx(fnvc("NtCreateThreadEx")), &thread, 0x1FFFFF, 0, m_proc, reinterpret_cast<LPTHREAD_START_ROUTINE>(m_unloader_code_addr), m_unloader_data_addr, 0, nullptr, nullptr, nullptr, nullptr); status != STATUS_SUCCESS)
	//	return false;
	//
	//if (!thread)
	//	return false;
	//
	//LI_FN(WaitForSingleObject).cached()(thread, INFINITE);
	//LI_FN(CloseHandle).cached()(thread);
	//
	//m_loader_data_size = 0;
	//if (auto status = do_syscall<NTSTATUS>(ctx.m_syscall.get_idx(fnvc("NtFreeVirtualMemory")), m_proc, &m_loader_data_addr, &m_loader_data_size, MEM_RELEASE); status != STATUS_SUCCESS)
	//	return false;
	//
	//m_loader_code_size = 0;
	//if (auto status = do_syscall<NTSTATUS>(ctx.m_syscall.get_idx(fnvc("NtFreeVirtualMemory")), m_proc, &m_loader_code_addr, &m_loader_code_size, MEM_RELEASE); status != STATUS_SUCCESS)
	//	return false;
	//
	//m_unloader_data_size = 0;
	//if (auto status = do_syscall<NTSTATUS>(ctx.m_syscall.get_idx(fnvc("NtFreeVirtualMemory")), m_proc, &m_unloader_data_addr, &m_unloader_data_size, MEM_RELEASE); status != STATUS_SUCCESS)
	//	return false;
	//
	//m_unloader_code_size = 0;
	//if (auto status = do_syscall<NTSTATUS>(ctx.m_syscall.get_idx(fnvc("NtFreeVirtualMemory")), m_proc, &m_unloader_code_addr, &m_unloader_code_size, MEM_RELEASE); status != STATUS_SUCCESS)
	//	return false;
	//
	//m_cheat_size = 0;
	//if (auto status = do_syscall<NTSTATUS>(ctx.m_syscall.get_idx(fnvc("NtFreeVirtualMemory")), m_proc, &m_cheat_addr, &m_cheat_size, MEM_RELEASE); status != STATUS_SUCCESS)
	//	return false;
	//
	return true;
}
