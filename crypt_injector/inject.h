#pragma once
#include "syscall.h"

class c_inject
{
	c_memory m_memory;
	CLIENT_ID find_process();

	HANDLE m_proc = nullptr;

	void* m_loader_stub = nullptr;
	size_t m_loader_size = 0;
	
	void* m_cheat_base = nullptr;
	size_t m_cheat_size = 0;

	uintptr_t m_entry_point = 0;
	uint8_t* m_raw_data = nullptr;

	std::unordered_map<uint64_t, uintptr_t> m_imports;

	static uint64_t* get_ptr_from_rva(uint64_t rva, IMAGE_NT_HEADERS* nt_header, uint8_t* image_base);
	static PIMAGE_SECTION_HEADER get_enclosing_section_header(uint64_t rva, PIMAGE_NT_HEADERS nt_header);

	void solve_imports(uint8_t* base, IMAGE_NT_HEADERS* nt_header, IMAGE_IMPORT_DESCRIPTOR* import_descriptor);
	void solve_relocations(uint64_t base, uint64_t relocation_base, IMAGE_NT_HEADERS* nt_header, IMAGE_BASE_RELOCATION* reloc, size_t size);
	void map_pe_sections(uint64_t base, IMAGE_NT_HEADERS* nt_header);
	bool parse_imports();
public:
	bool inject_from_memory(uint8_t* dll);
	bool unload();
};