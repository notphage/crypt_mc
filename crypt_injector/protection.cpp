#include "context.h"
#include <array>
#include <psapi.h>

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
		return false;
	}

	pe32.dwSize = sizeof(PROCESSENTRY32);

	if (!Process32First(snapshot, &pe32))
	{
		CloseHandle(snapshot);
		m_err_str = xors("UNSPECIFIED SYSTEM ERROR 2 ");
		m_err_str += std::to_string(GetLastError());
		m_status = STATUS_PROGRAM_NOSNAPSHOT;
		return false;
	}

	do
	{
		if (strstr(pe32.szExeFile, xors("idaq")))
		{
			CloseHandle(snapshot);
			m_err_str = xors("Please close IDA");
			m_status = STATUS_UNSAFE_SYSTEM;
			return false;
		}

		if (strstr(pe32.szExeFile, xors("Wireshark")))
		{
			CloseHandle(snapshot);
			m_err_str = xors("Please close Wireshark");
			m_status = STATUS_UNSAFE_SYSTEM;
			return false;
		}

		if (strstr(pe32.szExeFile, xors("ProcessHacker")))
		{
			CloseHandle(snapshot);
			m_err_str = xors("Please close Process Hacker");
			m_status = STATUS_UNSAFE_SYSTEM;
			return false;
		}

		if (strstr(pe32.szExeFile, xors("GlassWire")))
		{
			CloseHandle(snapshot);
			m_err_str = xors("Please close GlassWire");
			m_status = STATUS_UNSAFE_SYSTEM;
			return false;
		}
	} while (Process32Next(snapshot, &pe32));

	CloseHandle(snapshot);

	return true;
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
					return false;
				}
				
				if (strstr(szDriver, xors("npcap")))
				{
					m_err_str = xors("Please disable Wireshark");
					m_status = STATUS_UNSAFE_SYSTEM;
					return false;
				}

				if (strstr(szDriver, xors("gwdrv")))
				{
					m_err_str = xors("Please disable GlassWire");
					m_status = STATUS_UNSAFE_SYSTEM;
					return false;
				}

				if (strstr(szDriver, xors("kprocesshacker")))
				{
					m_err_str = xors("Please disable Process Hacker");
					m_status = STATUS_UNSAFE_SYSTEM;
					return false;
				}
			}
		}
	}
	else
	{
		m_err_str = xors("UNSPECIFIED SYSTEM ERROR 3 ");
		m_err_str += std::to_string(GetLastError());
		m_status = STATUS_DRIVER_NOENUM;
		return false;
	}

	return true;
}

bool c_protection::is_running_forbidden_program()
{
	return false;
}

bool c_protection::is_running_forbidden_driver()
{
	return false;
}
