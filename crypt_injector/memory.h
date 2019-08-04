#pragma once
#include "syscall.h"

class c_memory
{
	HANDLE m_proc = nullptr;
	c_syscall m_syscall;
public:
	c_memory();

	bool open(const CLIENT_ID& id);
	bool alloc(void*& addr, size_t* size, int32_t alloc_type, int32_t protect);
	bool thread(HANDLE& thread, void* entry, void* arg);
	void close();
	HMODULE module(const char* mod);
	uintptr_t proc_addr(const char* mod, const char* func);

	template <typename T>
	bool write(void* addr, const T& value, size_t size)
	{
		auto ret = do_syscall<NTSTATUS>(m_syscall.get_idx(fnvc("NtWriteVirtualMemory")), m_proc, addr, value, size, nullptr);
		return ret == STATUS_SUCCESS;
	}

	bool read(void* addr, void* buffer, size_t size)
	{
		auto ret = do_syscall<NTSTATUS>(m_syscall.get_idx(fnvc("NtReadVirtualMemory")), m_proc, addr, buffer, size, nullptr);
		return ret == STATUS_SUCCESS;
	}
};
