#include "context.h"
#include <TlHelp32.h>
#include "Shlwapi.h"
#include <psapi.h>
#include <filesystem>

c_memory::c_memory()
{
	m_syscall.init();
}

bool c_memory::open(const CLIENT_ID& id)
{
	static OBJECT_ATTRIBUTES zoa = { sizeof(zoa) };
	if (auto status = do_syscall<NTSTATUS>(ctx.m_syscall.get_idx(fnvc("NtOpenProcess")), &m_proc, PROCESS_ALL_ACCESS, &zoa, &id); status != STATUS_SUCCESS)
		return false;

	if (!m_proc)
		return false;

	m_client_id = id;

	return true;
}

bool c_memory::alloc(void*& addr, size_t* size, int32_t alloc_type, int32_t protect)
{
	if (auto status = do_syscall<NTSTATUS>(ctx.m_syscall.get_idx(fnvc("NtAllocateVirtualMemory")), m_proc, &addr, 0, size, alloc_type, protect); status != STATUS_SUCCESS)
		return false;

	if (!addr)
		return false;

	return true;
}

bool c_memory::thread(HANDLE& thread, void* entry, void* arg)
{
	auto status = do_syscall<NTSTATUS>(ctx.m_syscall.get_idx(fnvc("NtCreateThreadEx")), &thread, 0x1FFFFF, nullptr, m_proc, reinterpret_cast<LPTHREAD_START_ROUTINE>(entry), arg, 0, nullptr, nullptr, nullptr, nullptr);

	return status == STATUS_SUCCESS;
}

void c_memory::close()
{
	LI_FN(CloseHandle).cached()(m_proc);
}

HMODULE c_memory::module(const char* mod)
{
	HMODULE modules[1024];
	DWORD size_needed = 0;

	if (EnumProcessModules(m_proc, modules, sizeof modules, &size_needed))
	{
		for (auto i = 0; i < (size_needed / sizeof(HMODULE)); i++)
		{
			char mod_path[MAX_PATH];

			// Get the full path to the module's file.

			if (GetModuleFileNameEx(m_proc, modules[i], mod_path, sizeof(mod_path) / sizeof(char)))
			{
				auto module_name = std::string(PathFindFileName(mod_path));
				auto search_name = std::string(mod);

				std::transform(module_name.begin(), module_name.end(), module_name.begin(),
					[](unsigned char c) { return std::tolower(c); });
				
				std::transform(search_name.begin(), search_name.end(), search_name.begin(),
					[](unsigned char c) { return std::tolower(c); });

				if (module_name == search_name)
					return modules[i];
			}
		}
	}

	return nullptr;
}

//uintptr_t c_memory::proc_addr(const char* mod, const char* func)
//{
//	for (auto it = (char*)mod; *it != '\0'; ++it)
//		* it = tolower(static_cast<unsigned char>(*it));
//
//	auto remote_mod = module(mod);
//	auto local_mod = LI_FN(GetModuleHandleA).cached()(mod);
//
//	uintptr_t delta = (uintptr_t)remote_mod - (uintptr_t)local_mod;
//	auto local_func = LI_FN(GetProcAddress).cached()(local_mod, func);
//
//	return (uintptr_t)((uintptr_t)local_func + delta);
//}

uintptr_t c_memory::proc_addr(const char* mod, const char* func)
{
	for (auto it = (char*)mod; *it != '\0'; ++it)
		* it = tolower(static_cast<unsigned char>(*it));

	auto remote_mod = module(mod);
	if (!remote_mod)
		return 0;

	IMAGE_DOS_HEADER dos = {0};
	uint8_t hdr_nt32[sizeof(IMAGE_NT_HEADERS64)] = {0};
	auto nt32 = reinterpret_cast<PIMAGE_NT_HEADERS32>(hdr_nt32);
	auto nt64 = reinterpret_cast<PIMAGE_NT_HEADERS64>(hdr_nt32);
	DWORD exp_size = 0;
	uintptr_t exp_base = 0;

	if (!read((void*)remote_mod, &dos, sizeof(IMAGE_DOS_HEADER)) || dos.e_magic != IMAGE_DOS_SIGNATURE)
		return 0;

	if (!read((void*)((uintptr_t)remote_mod + dos.e_lfanew), &hdr_nt32, sizeof(IMAGE_NT_HEADERS)) || nt32->Signature != IMAGE_NT_SIGNATURE)
		return 0;

	if (nt32->OptionalHeader.Magic == IMAGE_NT_OPTIONAL_HDR32_MAGIC)
		exp_base = nt32->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
	else
		exp_base = nt64->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;

	if (exp_base)
	{
		if (nt32->OptionalHeader.Magic == IMAGE_NT_OPTIONAL_HDR32_MAGIC)
			exp_size = nt32->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size;
		else
			exp_size = nt64->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size;

		auto exp_data = (PIMAGE_EXPORT_DIRECTORY)(malloc(exp_size));
		if (!read((void*)((uintptr_t)remote_mod + exp_base), exp_data, exp_size))
			return 0;

		if (exp_size <= sizeof IMAGE_EXPORT_DIRECTORY)
		{
			exp_size = static_cast<uint32_t>(exp_data->AddressOfNameOrdinals - exp_base + std::max(exp_data->NumberOfFunctions, exp_data->NumberOfNames) * 255);

			free(exp_data);
			exp_data = (PIMAGE_EXPORT_DIRECTORY)(malloc(exp_size));
			if (!read((void*)((uintptr_t)remote_mod + exp_base), exp_data, exp_size))
				return 0;
		}

		uint16_t* ordn_addrs = reinterpret_cast<uint16_t*>(exp_data->AddressOfNameOrdinals + reinterpret_cast<uintptr_t>(exp_data) - exp_base);
		uint32_t* name_addrs = reinterpret_cast<uint32_t*>(exp_data->AddressOfNames + reinterpret_cast<uintptr_t>(exp_data) - exp_base);
		uint32_t* func_addrs = reinterpret_cast<uint32_t*>(exp_data->AddressOfFunctions + reinterpret_cast<uintptr_t>(exp_data) - exp_base);

		for (uint32_t i = 0; i < exp_data->NumberOfFunctions; ++i)
		{
			uint16_t ord_idx = 0xFFFF;
			char* func_name = nullptr;

			// Find by index
			if (reinterpret_cast<uintptr_t>(func) <= 0xFFFF)
				ord_idx = static_cast<uint16_t>(i);
			else if (reinterpret_cast<uintptr_t>(func) > 0xFFFF && i < exp_data->NumberOfFunctions)
			{
				func_name = (char*)(name_addrs[i] + reinterpret_cast<uintptr_t>(exp_data) - exp_base);
				ord_idx = static_cast<uint16_t>(ordn_addrs[i]);
			}
			else
				return 0;

			if ((reinterpret_cast<uintptr_t>(func) <= 0xFFFF && (uint16_t)((uintptr_t)func) == (ord_idx + exp_data->Base)) ||
				(reinterpret_cast<uintptr_t>(func) > 0xFFFF && strcmp(func_name, func) == 0))
			{
				uintptr_t proc_addr = func_addrs[ord_idx] + (uintptr_t)remote_mod;

				//if (proc_addr >= (uintptr_t)remote_mod + exp_base &&
				//	proc_addr <= (uintptr_t)remote_mod + exp_base + exp_size)
				//{
				//	MessageBoxA(nullptr, func_name, "Forwarded", MB_OK);
				//}

				return proc_addr;
			}
		}
	}

	return 0;
}
