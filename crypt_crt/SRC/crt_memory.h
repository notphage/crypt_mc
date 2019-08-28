#pragma once
#include <intrin.h>

using uint8_t = unsigned char;

//
// For whatever reason, these have to be declared as intrinsic.
// If not declared this way, compilation fails.
//
extern "C" void* memset(void* dst, int val, size_t size);
extern "C" void* memmove(void* dst, const void* src, size_t size);
extern "C" void* memcpy(void* dst, const void* src, size_t size);

#pragma intrinsic(memset)
#pragma intrinsic(memmove)
#pragma intrinsic(memcpy)