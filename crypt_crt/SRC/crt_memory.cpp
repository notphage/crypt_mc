#include "crt_memory.h"

//
// Our custom implementation of memset/memmove.
// I've opted for __stosb / __movsb due to the REPNE STOSB / REPNE MOVSB
// code output. Honestly, this is as good as we'll get in terms of size.
//
__declspec(noinline) void* __crt_memset(void* dst, uint8_t value, size_t size)
{
	uint8_t* dest = reinterpret_cast<uint8_t*>(dst);

	if (dest)
		__stosb(dest, value, size);

	return dst;
}

__declspec(noinline) void* __crt_memmove(void* dst, const void* src, size_t size)
{
	const uint8_t* source = reinterpret_cast<const uint8_t*>(src);
	uint8_t*       dest   = reinterpret_cast<uint8_t*>(dst);

	if (source && dest)
		__movsb(dest, source, size);

	return dst;
}

//
// And now we will wrap our custom CRT functions with the actual
// intrinsics that the compiler will place in the file.
//
#pragma function(memset)
#pragma function(memmove)
#pragma function(memcpy)

extern "C" void* memset(void* dst, int val, size_t size)          { return __crt_memset(dst, static_cast<uint8_t>(val), size); }
extern "C" void* memmove(void* dst, const void* src, size_t size) { return __crt_memmove(dst, src, size); }
extern "C" void* memcpy(void* dst, const void* src, size_t size)  { return __crt_memmove(dst, src, size); }