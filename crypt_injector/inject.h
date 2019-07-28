#pragma once
#include "syscall.h"

class c_inject
{
	CLIENT_ID find_process();
	bool write_virtual_memory(HANDLE handle, void* addr, void* buf, size_t size);
public:
	bool inject_from_memory(uint8_t* dll);
};