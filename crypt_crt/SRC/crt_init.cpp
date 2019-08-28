#include "crt_init.h"
#include <malloc.h>

// Define the start of our constructor section...
#pragma section(".CRT$XCA", read)
#pragma data_seg(".CRT$XCA")
ConstructorFn __xc_a[] = { NULL };

// ... as well as the end.
#pragma section(".CRT$XCZ", read)
#pragma data_seg(".CRT$XCZ")
ConstructorFn __xc_z[] = { NULL };

// Finally, merge the section we've just created with read-only data.
#pragma data_seg()
#pragma comment(linker, "/MERGE:.CRT=.rdata")

// Calls each constructor/destructor from first to last.
__declspec(noinline) void __cdecl _initterm(ConstructorFn* first, ConstructorFn* last)
{
	for (; first != last; ++first)
	{
		if (*first == nullptr)
			continue;

		(**first)();
	}
}

//
// Our atexit table and related functionality.
//
static DestructorFn* __crt_atexit_table       = nullptr;
static size_t        __crt_atexit_max_entries = 0;
static size_t        __crt_atexit_cur_entries = 0;

__declspec(noinline) void __cdecl _atexit_init()
{
	// We will give ourselves a generous amount of destructors.
	// If there's any issues regarding this, simply change the number.
	__crt_atexit_max_entries = 128;
	
	if (!__crt_atexit_table)
		__crt_atexit_table = reinterpret_cast<DestructorFn*>(
			calloc(__crt_atexit_max_entries, sizeof(DestructorFn*)));
}

__declspec(noinline) int __cdecl atexit(DestructorFn destructor)
{
	if (__crt_atexit_cur_entries < __crt_atexit_max_entries)
		__crt_atexit_table[__crt_atexit_cur_entries++] = destructor;

	return 0;
}

__declspec(noinline) void __cdecl _DoExit()
{
	if (__crt_atexit_cur_entries <= 0)
		return;

	_initterm(__crt_atexit_table, __crt_atexit_table + __crt_atexit_cur_entries);
}