#pragma once
#include "syscall.h"

class c_inject
{
	c_memory m_memory;
	CLIENT_ID find_process();

	uintptr_t rva_to_offset(uintptr_t rva, PIMAGE_SECTION_HEADER section_raw, size_t count);
	
	bool fix_imports(uint8_t* dll);
	bool rebase_image(uint8_t* dll, uintptr_t img_ptr);
public:
	bool inject_from_memory(uint8_t* dll);
};