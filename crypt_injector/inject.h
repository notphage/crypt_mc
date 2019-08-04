#pragma once
#include "syscall.h"

class c_inject
{
	CLIENT_ID find_process();
public:
	bool inject_from_memory(uint8_t* dll);
};