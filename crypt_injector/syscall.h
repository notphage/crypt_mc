#pragma once
#include <map>
#include <Windows.h>
#include <winternl.h>

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

extern "C" void* syscaller_stub();

namespace detail
{
	template <typename... Args>
	static inline auto syscall_stub_helper(
		Args... args
	) -> void*
	{
		auto fn = reinterpret_cast<void* (*)(Args...)>(&syscaller_stub);
		return fn(args...);
	}

	template <std::size_t argc, typename>
	struct argument_remapper
	{
		// At least 5 params
		template<
			typename First,
			typename Second,
			typename Third,
			typename Fourth,
			typename... Pack
		>
			static auto do_call(
				std::uint32_t idx,
				First first,
				Second second,
				Third third,
				Fourth fourth,
				Pack... pack
			) -> void*
		{
			return syscall_stub_helper(first, second, third, fourth, idx, nullptr, pack...);
		}
	};

	template <std::size_t Argc>
	struct argument_remapper<Argc, std::enable_if_t<Argc <= 4>>
	{
		// 4 or less params
		template<
			typename First = void*,
			typename Second = void*,
			typename Third = void*,
			typename Fourth = void*
		>
			static auto do_call(
				std::uint32_t idx,
				First first = First{},
				Second second = Second{},
				Third third = Third{},
				Fourth fourth = Fourth{}
			) -> void*
		{
			return syscall_stub_helper(first, second, third, fourth, idx, nullptr);
		}
	};
}

template<typename Return, typename... Args>
static inline auto do_syscall(
	uint32_t idx,
	Args... args
) -> Return
{
	using mapper = detail::argument_remapper<sizeof...(Args), void>;
	return (Return)mapper::do_call(idx, args...);
}

class c_syscall
{
	std::map<uintptr_t, uint32_t> syscalls;

	uint8_t* load_ntdll();
public:
	bool init();

	__forceinline uint32_t get_idx(uintptr_t str)
	{
		return syscalls[str];
	}
};
