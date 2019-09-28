#include "context.h"
#include <array>
#include <psapi.h>
#include <winsock2.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <stdlib.h>
#pragma comment(lib, "IPHLPAPI.lib")

#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))

#pragma pack(push,8) /* Work around a bug in tlhelp32.h in WIN64, which generates the wrong structure if packing has been changed */
#include <tlhelp32.h>
#pragma pack(pop)

#ifndef TH32CS_SNAPNOHEAPS
// define missing in Tlhelp32.h, but needed 
#define TH32CS_SNAPNOHEAPS 0x40000000
#endif

bool c_protection::is_debugger_present()
{
	if (IsDebuggerPresent())
	{
		m_err_str = xors("STATUS_WINAPI_DEBUGGER");
		m_status = STATUS_WINAPI_DEBUGGER;
		return true;
	}

	BOOL bIsDbgPresent = FALSE;
	CheckRemoteDebuggerPresent(GetCurrentProcess(), &bIsDbgPresent);
	if (bIsDbgPresent)
	{
		m_err_str = xors("STATUS_WINAPI_REMOTE_DEBUGGER");
		m_status = STATUS_WINAPI_REMOTE_DEBUGGER;
		return true;
	}

	PPEB pPeb = reinterpret_cast<PPEB>(__readgsqword(0x60));
	if (pPeb->BeingDebugged == 1)
	{
		m_err_str = xors("STATUS_PEB_DEBUGGER");
		m_status = STATUS_PEB_DEBUGGER;
		return true;
	}

	static PCONTEXT ctx = PCONTEXT(VirtualAlloc(nullptr, sizeof(ctx), MEM_COMMIT, PAGE_READWRITE));
	SecureZeroMemory(ctx, sizeof(CONTEXT));
	ctx->ContextFlags = CONTEXT_DEBUG_REGISTERS;

	if (GetThreadContext(GetCurrentThread(), ctx) == 0)
	{
		m_err_str = xors("STATUS_THREAD_DEBUGGER");
		m_status = STATUS_THREAD_DEBUGGER;
		return true;
	}

	if (ctx->Dr0 != 0 || ctx->Dr1 != 0 || ctx->Dr2 != 0 || ctx->Dr3 != 0)
	{
		m_err_str = xors("STATUS_THREAD_REGISTER_DEBUGGER");
		m_status = STATUS_THREAD_REGISTER_DEBUGGER;
		return true;
	}

	const uint8_t b = *reinterpret_cast<uint8_t*>(0x7ffe02d4);
	if ((b & 0x01) || (b & 0x02))
	{
		m_err_str = xors("STATUS_KUSER_KERNEL_DEBUGGER");
		m_status = STATUS_KUSER_KERNEL_DEBUGGER;
		return true;
	}

	return false;
}

bool c_protection::is_running_warning_program()
{
	PROCESSENTRY32 pe32 = {0};
	RtlSecureZeroMemory(&pe32, sizeof PROCESSENTRY32);

	const auto snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS | TH32CS_SNAPNOHEAPS, 0);

	if (snapshot == INVALID_HANDLE_VALUE)
	{
		m_err_str = xors("UNSPECIFIED SYSTEM ERROR 1 ");
		m_err_str += std::to_string(GetLastError());
		m_status = STATUS_PROGRAM_NOSNAPSHOT;
		return true;
	}

	pe32.dwSize = sizeof(PROCESSENTRY32);

	if (!Process32First(snapshot, &pe32))
	{
		CloseHandle(snapshot);
		m_err_str = xors("UNSPECIFIED SYSTEM ERROR 2 ");
		m_err_str += std::to_string(GetLastError());
		m_status = STATUS_PROGRAM_NOSNAPSHOT;
		return true;
	}

	do
	{
		if (strstr(pe32.szExeFile, xors("idaq")))
		{
			CloseHandle(snapshot);
			m_err_str = xors("Please close IDA");
			m_status = STATUS_UNSAFE_SYSTEM;
			return true;
		}

		if (strstr(pe32.szExeFile, xors("Wireshark")))
		{
			CloseHandle(snapshot);
			m_err_str = xors("Please close Wireshark");
			m_status = STATUS_UNSAFE_SYSTEM;
			return true;
		}

		if (strstr(pe32.szExeFile, xors("ProcessHacker")))
		{
			CloseHandle(snapshot);
			m_err_str = xors("Please close Process Hacker");
			m_status = STATUS_UNSAFE_SYSTEM;
			return true;
		}

		if (strstr(pe32.szExeFile, xors("GlassWire")))
		{
			CloseHandle(snapshot);
			m_err_str = xors("Please close GlassWire");
			m_status = STATUS_UNSAFE_SYSTEM;
			return true;
		}
	} while (Process32Next(snapshot, &pe32));

	CloseHandle(snapshot);

	return false;
}

bool c_protection::is_running_warning_driver()
{
	LPVOID drivers[1024];
	DWORD cbNeeded = 0;

	if (EnumDeviceDrivers(drivers, sizeof(drivers), &cbNeeded) && cbNeeded < sizeof(drivers))
	{
		TCHAR szDriver[1024];
		const int cDrivers = cbNeeded / sizeof(drivers[0]);
		for (int i = 0; i < cDrivers; i++)
		{
			if (GetDeviceDriverBaseName(drivers[i], szDriver, sizeof(szDriver) / sizeof(szDriver[0])))
			{
				if (strstr(szDriver, xors("Sbie")))
				{
					m_err_str = xors("Please disable Sandboxie");
					m_status = STATUS_UNSAFE_SYSTEM;
					return true;
				}
				
				if (strstr(szDriver, xors("npcap")))
				{
					m_err_str = xors("Please disable Wireshark");
					m_status = STATUS_UNSAFE_SYSTEM;
					return true;
				}

				if (strstr(szDriver, xors("gwdrv")))
				{
					m_err_str = xors("Please disable GlassWire");
					m_status = STATUS_UNSAFE_SYSTEM;
					return true;
				}

				if (strstr(szDriver, xors("kprocesshacker")))
				{
					m_err_str = xors("Please disable Process Hacker");
					m_status = STATUS_UNSAFE_SYSTEM;
					return true;
				}
			}
		}
	}
	else
	{
		m_err_str = xors("UNSPECIFIED SYSTEM ERROR 3 ");
		m_err_str += std::to_string(GetLastError());
		m_status = STATUS_DRIVER_NOENUM;
		return true;
	}

	return false;
}

bool c_protection::is_running_forbidden_program()
{
	std::string szProcesses[] = {
		xors("ebug"),
		xors("dbg"),
		xors("idaq"),
		xors("regmon"),
		xors("nalyzer"),
		xors("ireshark"),
		xors("HookExplorer"),
		xors("PETools"),
		xors("LordPE"),
		xors("SysInspector"),
		xors("sniff_"),
		xors("procmon"),
		xors("dump"),
		xors("procexp"),
		xors("olly"),
		xors("tcpview"),
		xors("autoruns"),
		xors("ImportREC"),
		xors("joebox"),
		xors("acker"),
		xors("filemon"),
		xors("hunter"), //pc hunter
		xors("atengi"), //cheat engine
		xors("processhacker"),
	};

	WORD iLength = sizeof(szProcesses) / sizeof(szProcesses[0]);

	PROCESSENTRY32 pe32;
	SecureZeroMemory(&pe32, sizeof(PROCESSENTRY32));

	// We want a snapshot of processes
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	// Check for a valid handle, in this case we need to check for
	// INVALID_HANDLE_VALUE instead of NULL
	if (hSnapshot == INVALID_HANDLE_VALUE)
	{
		m_status = STATUS_PROGRAM_NOSNAPSHOT;
		m_err_str = xors("PROGRAM_NOSNAPSHOT");
		return true;
	}

	// Now we can enumerate the running process, also 
	// we can't forget to set the PROCESSENTRY32.dwSize member
	// otherwise the following functions will fail
	pe32.dwSize = sizeof(PROCESSENTRY32);

	if (!Process32First(hSnapshot, &pe32))
	{
		CloseHandle(hSnapshot);
		m_status = STATUS_PROGRAM_NOSNAPSHOT;
		m_err_str = xors("PROGRAM_NOSNAPSHOT");
		return true;
	}

	do
	{
		for (int i = 0; i < iLength; i++)
		{
			if (strstr(pe32.szExeFile, szProcesses[i].c_str()))
			{
				CloseHandle(hSnapshot);
				m_status = protection_status_t(STATUS_PROGRAM_EBUG + i);
				m_err_str = xors("PROGRAM_");
				m_err_str += pe32.szExeFile;
				return true;
			}
		}
	} while (Process32Next(hSnapshot, &pe32));

	CloseHandle(hSnapshot);
	return false;
}

bool c_protection::is_running_forbidden_driver()
{
	std::string szDrivers[] = {
		xors("Sbie"), //sandboxie
		xors("NPF"), //winpcap/wireshark
		xors("acker"), //process hacker
		xors("CEDRI"), //cheatengine
	};

	WORD iLength = sizeof(szDrivers) / sizeof(szDrivers[0]);

	LPVOID drivers[1024];
	DWORD cbNeeded;

	if (EnumDeviceDrivers(drivers, sizeof(drivers), &cbNeeded) && cbNeeded < sizeof(drivers))
	{
		TCHAR szDriver[1024];
		int cDrivers = cbNeeded / sizeof(drivers[0]);
		for (int i = 0; i < cDrivers; i++)
		{
			if (GetDeviceDriverBaseName(drivers[i], szDriver, sizeof(szDriver) / sizeof(szDriver[0])))
			{
				for (int j = 0; j < iLength; j++)
				{
					if (strstr(szDriver, szDrivers[j].c_str()))
					{
						m_status = protection_status_t(STATUS_DRIVER_SANDBOXIE + j);
						m_err_str = xors("DRIVER_");
						m_err_str += std::to_string(j);
						return true;
					}
				}
			}
		}
	}
	else
	{
		m_status = STATUS_DRIVER_NOENUM;
		m_err_str = xors("DRIVER_NOENUM");
		return true;
	}

	return false;
}

bool c_protection::has_forbidden_mac_address()
{
	std::string szMacs[] = {
		xors("\x00\x1C\x42"), //Parallels
		xors("\x00\x16\x3E"), //Xen
		xors("\x08\x00\x27"), //VirtualBox
		xors("\x0A\x00\x27"), //Hybrid Analysis
	};

	BOOL bResult = FALSE;
	PIP_ADAPTER_INFO pAdapterInfo, pAdapterInfoPtr;
	ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);

	pAdapterInfo = (PIP_ADAPTER_INFO)MALLOC(sizeof(IP_ADAPTER_INFO));
	if (pAdapterInfo == NULL)
	{
		m_status = STATUS_MAC_NOADAPTER;
		m_err_str = xors("MAC_NOADAPTER");
		return true;
	}

	DWORD dwResult = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen);

	// Make an initial call to GetAdaptersInfo to get the necessary size into the ulOutBufLen variable
	if (dwResult == ERROR_BUFFER_OVERFLOW)
	{
		FREE(pAdapterInfo);
		pAdapterInfo = (PIP_ADAPTER_INFO)MALLOC(ulOutBufLen);
		if (pAdapterInfo == NULL)
		{
			m_status = STATUS_MAC_NOADAPTER;
			m_err_str = xors("MAC_NOADAPTER");
			return true;
		}

		// Now, we can call GetAdaptersInfo
		dwResult = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen);
	}

	if (dwResult == ERROR_SUCCESS)
	{
		for (auto mac_iter = 0; mac_iter < szMacs->size(); mac_iter++)
		{
			// Convert the given mac address to an array of multibyte chars so we can compare.
			CHAR szMacMultiBytes[4];
			for (int i = 0; i < 4; i++) {
				szMacMultiBytes[i] = (CHAR)(szMacs[mac_iter].c_str())[i];
			}

			pAdapterInfoPtr = pAdapterInfo;

			while (pAdapterInfoPtr)
			{
				if (pAdapterInfoPtr->AddressLength == 6 && !memcmp(szMacMultiBytes, pAdapterInfoPtr->Address, 3))
				{
					m_status = protection_status_t(STATUS_MAC_PARALLELS + mac_iter);
					m_err_str = xors("MAC_");
					m_err_str += std::to_string(mac_iter);
					return true;
				}
				pAdapterInfoPtr = pAdapterInfoPtr->Next;
			}
		}
	}

	FREE(pAdapterInfo);

	return false;
}

bool c_protection::tables_remapped()
{
	// IDT
	{
		unsigned char idtr[10]{};
		unsigned long idt = 0;

		// sidt instruction stores the contents of the IDT Register
		// (the IDTR which points to the IDT) in a processor register.
		__sidt(&idtr);

		if (idtr == nullptr)
		{
			m_status = STATUS_TABLE_IDT;
			m_err_str = xors("TABLE_IDT");
			return true;
		}

		idt = *((unsigned long*)&idtr[2]);

		if ((idt >> 24) == 0xff)
		{
			m_status = STATUS_TABLE_IDT;
			m_err_str = xors("TABLE_IDT");
			return true;
		}
	}

	// GDT
	{
		unsigned char gdtr[10]{};
		unsigned long gdt = 0;

		// sgdt instruction stores the contents of the GDT Register
		// (the GDTR which points to the GDT) in a processor register.
		_sgdt(&gdtr);

		if (gdtr == nullptr)
		{
			m_status = STATUS_TABLE_GDT;
			m_err_str = xors("TABLE_GDT");
			return true;
		}

		gdt = *((unsigned long*)&gdtr[2]);

		if ((gdt >> 24) == 0xff)
		{
			m_status = STATUS_TABLE_GDT;
			m_err_str = xors("TABLE_GDT");
			return true;
		}
	}

	return false;
}

bool c_protection::hypervisor_present()
{
	// Hypervisor Bit
	{
		int CPUInfo[4] = { -1 };

		/* Query hypervisor precense using CPUID (EAX=1), BIT 31 in ECX */
		__cpuid(CPUInfo, 1);
		if ((CPUInfo[2] >> 31) & 1)
		{
			m_status = STATUS_CPUID_BIT;
			m_err_str = xors("CPUID_BIT");
			return true;
		}
	}

	// CPUID Timing
	{
		ULONGLONG tsc1 = 0;
		ULONGLONG tsc2 = 0;
		ULONGLONG avg = 0;
		INT cpuInfo[4] = {};

		// Try this 10 times in case of small fluctuations
		for (INT i = 0; i < 10; i++)
		{
			tsc1 = __rdtsc();
			__cpuid(cpuInfo, 0);
			tsc2 = __rdtsc();

			// Get the delta of the two RDTSC
			avg += (tsc2 - tsc1);
		}

		// We repeated the process 10 times so we make sure our check is as much reliable as we can
		avg = avg / 10;
		if (avg > 1000)
		{
			m_status = STATUS_TIMING_CPUID;
			m_err_str = xors("TIMING_CPUID");
			return true;
		}
	}

	return false;
}
