#pragma once
#include <map>
#include <Windows.h>
#include <winternl.h>

class c_stub
{
	uint8_t m_stub[11] =
	{
		0x4C, 0x8B, 0xD1,                               // mov r10 rcx
		0xB8, 0x00, 0x00, 0x00, 0x00,                   // eax syscall index
		0x0F, 0x05,                                     // do syscall
		0xC3                                            // retn
	};

public:
	void set_index(uint32_t syscall_index)
	{
		DWORD old_flag;
		VirtualProtect(m_stub, 11, PAGE_EXECUTE_READWRITE, &old_flag);
		*(uint32_t*)(&m_stub[4]) = syscall_index;
	}

	__forceinline bool validate() const
	{
		return *(uint32_t*)(&m_stub[4]) != 0;
	}

	__forceinline uintptr_t addr() const
	{
		return uintptr_t(m_stub);
	}
};

enum MEMORY_INFORMATION_CLASS
{
	MemoryBasicInformation,
	MemoryWorkingSetList,
	MemorySectionName,
	MemoryBasicVlmInformation
};

// memory
using ZwReadVirtualMemory = NTSTATUS(__stdcall*)(HANDLE, const void*, void*, uint32_t, size_t*);
using ZwWriteVirtualMemory = NTSTATUS(__stdcall*)(HANDLE, void*, void*, uint32_t, size_t*);
using ZwAllocateVirtualMemory = NTSTATUS(__stdcall*)(HANDLE, void**, unsigned long, size_t*, uint32_t, uint32_t);
using ZwFreeVirtualMemory = NTSTATUS(__stdcall*)(HANDLE, void**, size_t*, uint32_t);
using ZwProtectVirtualMemory = NTSTATUS(__stdcall*)(HANDLE, void**, size_t*, uint32_t, uint32_t*);
using ZwQueryVirtualMemory = NTSTATUS(__stdcall*)(HANDLE, void*, MEMORY_INFORMATION_CLASS, void*, size_t, size_t*);

// handle
using ZwOpenProcess = NTSTATUS(__stdcall*)(HANDLE*, ACCESS_MASK, const OBJECT_ATTRIBUTES*, const CLIENT_ID*);
using ZwClose = NTSTATUS(__stdcall*)(HANDLE);

// thread
using ZwCreateThreadEx = NTSTATUS(__stdcall*)(void**, ACCESS_MASK, const OBJECT_ATTRIBUTES*, HANDLE, LPTHREAD_START_ROUTINE, void*, int, uint32_t, uint32_t, uint32_t, void*);
using ZwOpenThread = NTSTATUS(__stdcall*)(void**, ACCESS_MASK, POBJECT_ATTRIBUTES, CLIENT_ID*);
using ZwSuspendThread = NTSTATUS(__stdcall*)(void*, void*);
using ZwResumeThread = NTSTATUS(__stdcall*)(void*, void*);
using ZwGetContextThread = NTSTATUS(__stdcall*)(void*, CONTEXT*);
using ZwSetContextThread = NTSTATUS(__stdcall*)(void*, CONTEXT*);

// BSOD mofo, just call it with 5th parameter as int 6
using ZwRaiseHardError = NTSTATUS(__stdcall*)(NTSTATUS, uint32_t, uint32_t, void**, int, void*);

class c_syscall
{
	std::map<uintptr_t, c_stub> syscalls;

	uint8_t* load_ntdll();
public:
	bool init();

	template <typename fn>
	fn get(uintptr_t func_name)
	{
		return (fn)(syscalls[func_name].addr());
	}

};