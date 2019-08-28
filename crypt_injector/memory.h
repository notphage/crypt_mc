#pragma once
#include "syscall.h"

class c_memory
{
	CLIENT_ID m_client_id{};
	HANDLE m_proc = nullptr;
	HMODULE m_base = nullptr;
	c_syscall m_syscall;
public:
	c_memory();

	bool open(const CLIENT_ID& id);
	bool alloc(void*& addr, size_t* size, int32_t alloc_type, int32_t protect);
	bool protect(void*& addr, size_t* size, int32_t prot_type, int32_t& old_prot);
	bool thread(HANDLE& thread, void* entry, void* arg);
	void close() const;
	HMODULE module(const char* mod) const;
	uintptr_t proc_addr(const char* mod, const char* func);

	template <class Fn>
	bool enumerate_processes(Fn callback)
	{
		constexpr auto size_mismatch = 0xC0000004;
		constexpr auto process_type = SystemProcessInformation;

		auto allocation = std::make_unique<std::byte[]>(0x1);
		std::uint32_t size_needed = 0;
		const auto size_needed_ptr = reinterpret_cast<DWORD*>(&size_needed);

		auto ret = do_syscall<NTSTATUS>(m_syscall.get_idx(fnvc("NtQuerySystemInformation")), process_type, allocation.get(), 0x1, size_needed_ptr);
		if (ret != size_mismatch)
			return false;

		allocation = std::make_unique<std::byte[]>(size_needed);
		auto info = reinterpret_cast<SYSTEM_PROCESS_INFORMATION*>(allocation.get());

		ret = do_syscall<NTSTATUS>(m_syscall.get_idx(fnvc("NtQuerySystemInformation")), process_type, info, size_needed, size_needed_ptr);
		if (ret != STATUS_SUCCESS)
			return false;

		for (auto info_casted = reinterpret_cast<std::uintptr_t>(info); info->NextEntryOffset;
			info = reinterpret_cast<SYSTEM_PROCESS_INFORMATION*>(info_casted + info->NextEntryOffset),
			info_casted = reinterpret_cast<uintptr_t>(info))
		{
			if (callback(info))
				return true;
		}

		return false;
	}

	template <class Fn>
	bool enumerate_threads(Fn callback)
	{
		return enumerate_processes([=](SYSTEM_PROCESS_INFORMATION* info)
			{
				const auto info_casted = reinterpret_cast<std::uintptr_t>(info);
				const auto thread_info = reinterpret_cast<SYSTEM_THREAD_INFORMATION*>(info_casted + sizeof(SYSTEM_PROCESS_INFORMATION));
				for (uint32_t thread_index = 0; thread_index < info->NumberOfThreads; ++thread_index)
				{
					if (callback(&thread_info[thread_index]))
						return true;
				}

				return false;
			});
	}

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

	HMODULE get_base() const
	{
		return m_base;
	}

	CLIENT_ID get_client_id() const
	{
		return m_client_id;
	}
};
