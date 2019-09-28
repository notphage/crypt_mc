#include "ntdll.hpp"

namespace ntdll
{
	NtCreateSection_t NtCreateSection = nullptr;
	NtMapViewOfSection_t NtMapViewOfSection = nullptr;
	NtUnmapViewOfSection_t NtUnmapViewOfSection = nullptr;
	NtClose_t NtClose = nullptr;
	NtQuerySystemInformation_t NtQuerySystemInformation = nullptr;

	void initialise() noexcept
	{
		const auto module_handle = GetModuleHandleA("ntdll.dll");
		NtCreateSection = reinterpret_cast<NtCreateSection_t>(GetProcAddress(module_handle, "NtCreateSection"));
		NtMapViewOfSection = reinterpret_cast<NtMapViewOfSection_t>(GetProcAddress(module_handle, "NtMapViewOfSection"));
		NtUnmapViewOfSection = reinterpret_cast<NtUnmapViewOfSection_t>(GetProcAddress(module_handle, "NtUnmapViewOfSection"));
		NtClose = reinterpret_cast<NtClose_t>(GetProcAddress(module_handle, "NtClose"));
		NtQuerySystemInformation = reinterpret_cast<NtQuerySystemInformation_t>(GetProcAddress(module_handle, "NtQuerySystemInformation"));
	}
}
