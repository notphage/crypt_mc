#include <Windows.h>

typedef struct _iobuf
{
	void* _Placeholder;
} FILE;

extern "C" FILE* __cdecl fopen(const char* file, const char* mode)
{
	static auto pFopen = (FILE*(*)(const char*, const char*))GetProcAddress(GetModuleHandleA("ucrtbase.dll"), "fopen");

	return pFopen(file, mode);
}

extern "C" int __cdecl fclose(FILE* stream)
{
	static auto pFclose = (int(*)(FILE*))GetProcAddress(GetModuleHandleA("ucrtbase.dll"), "fclose");

	return pFclose(stream);
}

extern "C" unsigned long long __cdecl fread(void* ptr, unsigned long long size, unsigned long long count, FILE* stream)
{
	static auto pFread = (unsigned long long(*)(void*, unsigned long long, unsigned long long, FILE*))GetProcAddress(GetModuleHandleA("ucrtbase.dll"), "fread");

	return pFread(ptr, size, count, stream);
}

extern "C" int __cdecl fseek(FILE* stream, long int offset, int origin)
{
	static auto pFseek = (int(*)(FILE*, long int, int))GetProcAddress(GetModuleHandleA("ucrtbase.dll"), "fseek");

	return pFseek(stream, offset, origin);
}

extern "C" long int __cdecl ftell(FILE* stream)
{
	static auto pFtell = (long int(*)(FILE*))GetProcAddress(GetModuleHandleA("ucrtbase.dll"), "ftell");

	return pFtell(stream);
}
