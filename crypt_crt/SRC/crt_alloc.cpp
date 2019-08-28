#include <windows.h>

//
// Implementation of memory allocation. Since we don't have access
// to 'malloc' and 'free', we have two options:
//  a) dynamically link to msvcrt.lib and import them
//  b) use the heap functionality exposed by Windows
//

static bool   gHeapInit = false;
static HANDLE gHeap     = nullptr;

__declspec(noinline) void __crt_init_heap()
{
	if (gHeapInit)
		return;

	gHeap = GetProcessHeap();
}

__declspec(noinline) void* __crt_heap_alloc(size_t alloc_size)
{
	if (!gHeapInit)
		__crt_init_heap();

	return HeapAlloc(gHeap, NULL, alloc_size);
}

__declspec(noinline) void* __crt_heap_realloc(void* alloc, size_t alloc_size)
{
	if (!gHeapInit)
		__crt_init_heap();

	if (alloc)
		return HeapReAlloc(gHeap, NULL, alloc, alloc_size);

	return HeapAlloc(gHeap, NULL, alloc_size);
}

__declspec(noinline) size_t __crt_heap_size(void* alloc)
{
	if (!gHeapInit)
		__crt_init_heap();

	return HeapSize(gHeap, NULL, alloc);
}

__declspec(noinline) void __crt_heap_free(void* alloc)
{
	if (!gHeapInit)
		__crt_init_heap();

	HeapFree(gHeap, NULL, alloc);
}

//
// Now, we will wrap the standard C library implementations with the
// functions we created above.
//

extern "C" void* malloc(size_t alloc_size)               { return __crt_heap_alloc(alloc_size); }
extern "C" void* realloc(void* alloc, size_t alloc_size) { return __crt_heap_realloc(alloc, alloc_size); }
extern "C" void  free(void* alloc)                       { __crt_heap_free(alloc); }

extern "C" void* calloc(size_t items, size_t item_size)      { return __crt_heap_alloc(items * item_size); }
extern "C" void* _calloc_crt(size_t items, size_t item_size) { return __crt_heap_alloc(items * item_size); }

extern "C" void*  _nh_malloc(size_t alloc_size, int) { return __crt_heap_alloc(alloc_size); }
extern "C" size_t _msize(void* alloc)                { return __crt_heap_size(alloc); }

//
// Finally, we will overload the allocation operators (new/delete).
//

void* operator new(size_t alloc_size)
{
	return __crt_heap_alloc(alloc_size);
}

void* operator new[](size_t alloc_size)
{
	return __crt_heap_alloc(alloc_size);
}

void operator delete(void* alloc)
{
	__crt_heap_free(alloc);
}

void operator delete(void* alloc, size_t)
{
	__crt_heap_free(alloc);
}

void operator delete[](void* alloc)
{
	__crt_heap_free(alloc);
}

void operator delete[](void* alloc, size_t)
{
	__crt_heap_free(alloc);
}