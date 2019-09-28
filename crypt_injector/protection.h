#pragma once

enum protection_status_t
{
	STATUS_SAFE,
	STATUS_UNSAFE_SYSTEM,

	STATUS_WINAPI_DEBUGGER,
	STATUS_WINAPI_REMOTE_DEBUGGER,
	STATUS_PEB_DEBUGGER,
	STATUS_THREAD_DEBUGGER,
	STATUS_THREAD_REGISTER_DEBUGGER,
	STATUS_KUSER_KERNEL_DEBUGGER,

	STATUS_PROGRAM_NOSNAPSHOT,
	STATUS_PROGRAM_EBUG,
	STATUS_PROGRAM_DBG,
	STATUS_PROGRAM_IDAQ,
	STATUS_PROGRAM_REGMON,
	STATUS_PROGRAM_NALYZER,
	STATUS_PROGRAM_WIRESHARK,
	STATUS_PROGRAM_HOOKEXPLORER,
	STATUS_PROGRAM_PETOOLS,
	STATUS_PROGRAM_LORDPE,
	STATUS_PROGRAM_SYSINSPECTOR,
	STATUS_PROGRAM_SNIFFHIT,
	STATUS_PROGRAM_PROCMON,
	STATUS_PROGRAM_DUMP,
	STATUS_PROGRAM_PROCEXP,
	STATUS_PROGRAM_OLLY,
	STATUS_PROGRAM_TCPVIEW,
	STATUS_PROGRAM_AUTORUNS,
	STATUS_PROGRAM_IMPORTREC,
	STATUS_PROGRAM_JOEBOX,
	STATUS_PROGRAM_PROCESSHACKER,
	STATUS_PROGRAM_FILEMON,
	STATUS_PROGRAM_PCHUNTER,
	STATUS_PROGRAM_CHEATENGINE,

	STATUS_DRIVER_NOENUM,
	STATUS_DRIVER_SANDBOXIE,
	STATUS_DRIVER_WINPCAP,
	STATUS_DRIVER_PROCESS_HACKER,
	STATUS_DRIVER_CHEAT_ENGINE,

	STATUS_MAC_NOADAPTER,
	STATUS_MAC_PARALLELS,
	STATUS_MAC_XEN,
	STATUS_MAC_VBOX,
	STATUS_MAC_HYBRID_ANALYSIS,

	STATUS_TABLE_IDT,
	STATUS_TABLE_GDT,

	STATUS_CPUID_BIT,
	STATUS_TIMING_CPUID,
};

typedef struct _KSYSTEM_TIME
{
	ULONG LowPart;
	LONG High1Time;
	LONG High2Time;
} KSYSTEM_TIME, * PKSYSTEM_TIME;

typedef enum _NT_PRODUCT_TYPE
{
	NtProductWinNt = 1,
	NtProductLanManNt = 2,
	NtProductServer = 3
} NT_PRODUCT_TYPE;

typedef enum _ALTERNATIVE_ARCHITECTURE_TYPE
{
	StandardDesign = 0,
	NEC98x86 = 1,
	EndAlternatives = 2
} ALTERNATIVE_ARCHITECTURE_TYPE;

typedef struct _KUSER_SHARED_DATA
{
	ULONG TickCountLowDeprecated;
	ULONG TickCountMultiplier;
	KSYSTEM_TIME InterruptTime;
	KSYSTEM_TIME SystemTime;
	KSYSTEM_TIME TimeZoneBias;
	WORD ImageNumberLow;
	WORD ImageNumberHigh;
	WCHAR NtSystemRoot[260];
	ULONG MaxStackTraceDepth;
	ULONG CryptoExponent;
	ULONG TimeZoneId;
	ULONG LargePageMinimum;
	ULONG Reserved2[7];
	NT_PRODUCT_TYPE NtProductType;
	UCHAR ProductTypeIsValid;
	ULONG NtMajorVersion;
	ULONG NtMinorVersion;
	UCHAR ProcessorFeatures[64];
	ULONG Reserved1;
	ULONG Reserved3;
	ULONG TimeSlip;
	ALTERNATIVE_ARCHITECTURE_TYPE AlternativeArchitecture;
	LARGE_INTEGER SystemExpirationDate;
	ULONG SuiteMask;
	UCHAR KdDebuggerEnabled;
	UCHAR NXSupportPolicy;
	ULONG ActiveConsoleId;
	ULONG DismountCount;
	ULONG ComPlusPackage;
	ULONG LastSystemRITEventTickCount;
	ULONG NumberOfPhysicalPages;
	UCHAR SafeBootMode;
	ULONG SharedDataFlags;
	ULONG DbgErrorPortPresent : 1;
	ULONG DbgElevationEnabled : 1;
	ULONG DbgVirtEnabled : 1;
	ULONG DbgInstallerDetectEnabled : 1;
	ULONG SystemDllRelocated : 1;
	ULONG SpareBits : 27;
	UINT64 TestRetInstruction;
	ULONG SystemCall;
	ULONG SystemCallReturn;
	UINT64 SystemCallPad[3];
	union
	{
		KSYSTEM_TIME TickCount;
		UINT64 TickCountQuad;
	};
	ULONG Cookie;
	INT64 ConsoleSessionForegroundProcessId;
	ULONG Wow64SharedInformation[16];
	WORD UserModeGlobalLogger[8];
	ULONG HeapTracingPid[2];
	ULONG CritSecTracingPid[2];
	ULONG ImageFileExecutionOptions;
	union
	{
		UINT64 AffinityPad;
		ULONG ActiveProcessorAffinity;
	};
	UINT64 InterruptTimeBias;
} KUSER_SHARED_DATA, * PKUSER_SHARED_DATA;

static constexpr uintptr_t KUSER_SHARED_DATA_PTR = 0x7FFE0000;

class c_protection
{
	bool is_debugger_present();
	bool is_running_warning_program();
	bool is_running_warning_driver();
	
	bool is_running_forbidden_program();
	bool is_running_forbidden_driver();
	bool has_forbidden_mac_address();
	bool tables_remapped();
	bool hypervisor_present();

	KUSER_SHARED_DATA* m_kuser_shared_data = reinterpret_cast<KUSER_SHARED_DATA*>(KUSER_SHARED_DATA_PTR);
public:
	protection_status_t m_status = STATUS_SAFE;
	std::string m_err_str{};

	__declspec(noinline) bool init_safety_check()
	{
		// safe boot
		{
			if (m_kuser_shared_data->SafeBootMode == TRUE)
			{
				m_err_str = xors("Please disable safe boot mode");
				m_status = STATUS_UNSAFE_SYSTEM;
				return false;
			}
		}

		if (is_running_warning_program())
			return false;

		if (is_running_warning_driver())
			return false;

		return true;
	}

	__declspec(noinline) bool safety_check()
	{
		if (is_running_forbidden_program())
			return false;

		if (is_running_forbidden_program())
			return false;

		if (has_forbidden_mac_address())
			return false;

		if (tables_remapped())
			return false;

		if (hypervisor_present())
			return false;

		return true;
	}
};
