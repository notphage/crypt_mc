#include "context.h"

#include <strsafe.h>

//
// IDENTIFY data (from ATAPI driver source)
//

#pragma pack(1)

typedef struct _IDENTIFY_DATA {
	USHORT GeneralConfiguration;            // 00 00
	USHORT NumberOfCylinders;               // 02  1
	USHORT Reserved1;                       // 04  2
	USHORT NumberOfHeads;                   // 06  3
	USHORT UnformattedBytesPerTrack;        // 08  4
	USHORT UnformattedBytesPerSector;       // 0A  5
	USHORT SectorsPerTrack;                 // 0C  6
	USHORT VendorUnique1[3];                // 0E  7-9
	USHORT SerialNumber[10];                // 14  10-19
	USHORT BufferType;                      // 28  20
	USHORT BufferSectorSize;                // 2A  21
	USHORT NumberOfEccBytes;                // 2C  22
	USHORT FirmwareRevision[4];             // 2E  23-26
	USHORT ModelNumber[20];                 // 36  27-46
	UCHAR  MaximumBlockTransfer;            // 5E  47
	UCHAR  VendorUnique2;                   // 5F
	USHORT DoubleWordIo;                    // 60  48
	USHORT Capabilities;                    // 62  49
	USHORT Reserved2;                       // 64  50
	UCHAR  VendorUnique3;                   // 66  51
	UCHAR  PioCycleTimingMode;              // 67
	UCHAR  VendorUnique4;                   // 68  52
	UCHAR  DmaCycleTimingMode;              // 69
	USHORT TranslationFieldsValid : 1;        // 6A  53
	USHORT Reserved3 : 15;
	USHORT NumberOfCurrentCylinders;        // 6C  54
	USHORT NumberOfCurrentHeads;            // 6E  55
	USHORT CurrentSectorsPerTrack;          // 70  56
	ULONG  CurrentSectorCapacity;           // 72  57-58
	USHORT CurrentMultiSectorSetting;       //     59
	ULONG  UserAddressableSectors;          //     60-61
	USHORT SingleWordDMASupport : 8;        //     62
	USHORT SingleWordDMAActive : 8;
	USHORT MultiWordDMASupport : 8;         //     63
	USHORT MultiWordDMAActive : 8;
	USHORT AdvancedPIOModes : 8;            //     64
	USHORT Reserved4 : 8;
	USHORT MinimumMWXferCycleTime;          //     65
	USHORT RecommendedMWXferCycleTime;      //     66
	USHORT MinimumPIOCycleTime;             //     67
	USHORT MinimumPIOCycleTimeIORDY;        //     68
	USHORT Reserved5[2];                    //     69-70
	USHORT ReleaseTimeOverlapped;           //     71
	USHORT ReleaseTimeServiceCommand;       //     72
	USHORT MajorRevision;                   //     73
	USHORT MinorRevision;                   //     74
	USHORT Reserved6[50];                   //     75-126
	USHORT SpecialFunctionsEnabled;         //     127
	USHORT Reserved7[128];                  //     128-255
} IDENTIFY_DATA, * PIDENTIFY_DATA;

#pragma pack()

void c_hwid::error(LPTSTR lpszFunction)
{
	LPVOID lpMsgBuf;
	LPVOID lpDisplayBuf;
	DWORD dw = GetLastError();

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)& lpMsgBuf,
		0, NULL);

	// Display the error message and exit the process

	lpDisplayBuf = (LPVOID)LocalAlloc(LMEM_ZEROINIT,
		(lstrlen((LPCTSTR)lpMsgBuf) + lstrlen((LPCTSTR)lpszFunction) + 40) * sizeof(TCHAR));
	StringCchPrintf((LPTSTR)lpDisplayBuf,
		LocalSize(lpDisplayBuf) / sizeof(TCHAR),
		TEXT("%s failed with error %d: %s"),
		lpszFunction, dw, lpMsgBuf);
	MessageBox(NULL, (LPCTSTR)lpDisplayBuf, TEXT("Error"), MB_OK);

	LocalFree(lpMsgBuf);
	LocalFree(lpDisplayBuf);
}

uint64_t c_hwid::disk_serials()
{
	uint64_t hwid_disk = 0;

	auto convert_to_str = [](DWORD disk_data[256], int first, int last, char* buf) -> char*
	{
		int index = 0;
		int position = 0;

		//  each integer has two characters stored in it backwards
		for (index = first; index <= last; index++)
		{
			//  get high byte for 1st character
			buf[position++] = (char)(disk_data[index] / 256);

			//  get low byte for 2nd character
			buf[position++] = (char)(disk_data[index] % 256);
		}

		//  end the string 
		buf[position] = '\0';

		//  cut off the trailing blanks
		for (index = position - 1; index > 0 && isspace(buf[index]); index--)
			buf[index] = '\0';

		return buf;
	};

	for (auto i = 0; i < 9; i++)
	{
		wchar_t disk_name[22];

		wsprintfW(disk_name, xors(L"\\\\.\\PhysicalDrive%i"), i);

		auto device = LI_FN(CreateFileW).cached()(disk_name, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, 0, nullptr);

		if (device != INVALID_HANDLE_VALUE)
		{
			size_t command_size = sizeof(SENDCMDINPARAMS) + IDENTIFY_BUFFER_SIZE;
			PSENDCMDINPARAMS cmd = (PSENDCMDINPARAMS)malloc(command_size);
			cmd->irDriveRegs.bCommandReg = 0xEC;
			DWORD bytes_ret = 0;

			if (!LI_FN(DeviceIoControl).cached()(device, SMART_RCV_DRIVE_DATA, cmd, sizeof(SENDCMDINPARAMS), cmd, command_size, &bytes_ret, nullptr))
			{
				LI_FN(CloseHandle).cached()(device);
			}
			else
			{
				DWORD diskdata[256];
				USHORT* pIdSector = (USHORT*)(PIDENTIFY_DATA)((PSENDCMDOUTPARAMS)cmd)->bBuffer;

				for (int ijk = 0; ijk < 256; ijk++)
					diskdata[ijk] = pIdSector[ijk];

				char serial[1024];
				char model[1024];
				convert_to_str(diskdata, 10, 19, serial);
				convert_to_str(diskdata, 27, 46, serial);

				hwid_disk ^= fnvr(serial);

				LI_FN(CloseHandle).cached()(device);
			}
		}
	}

	return hwid_disk;
}

typedef struct _SYSTEM_BOOT_ENVIRONMENT_INFORMATION
{
    GUID BootIdentifier;
    FIRMWARE_TYPE FirmwareType;
} SYSTEM_BOOT_ENVIRONMENT_INFORMATION, *PSYSTEM_BOOT_ENVIRONMENT_INFORMATION;

uint64_t c_hwid::boot_uuid()
{
	PSYSTEM_BOOT_ENVIRONMENT_INFORMATION boot_info;
	size_t needed = 8 * 1024 * 1024;

	boot_info = (PSYSTEM_BOOT_ENVIRONMENT_INFORMATION)malloc(needed);

	if (!boot_info)
		return 0;

	// SystemBootEnvironmentInformation = 90
	if (auto err = do_syscall<NTSTATUS>(ctx.m_syscall.get_idx(fnvc("NtQuerySystemInformation")), 90, boot_info, needed, 0); err != STATUS_SUCCESS)
		return 0;
	
	char serial[1024];
	sprintf(serial, "%08X-%04X-%04X-%02X%02X%02X%02X%02X%02X%02X%02X", boot_info->BootIdentifier.Data1, boot_info->BootIdentifier.Data2, boot_info->BootIdentifier.Data3, boot_info->BootIdentifier.Data4[0], boot_info->BootIdentifier.Data4[1], boot_info->BootIdentifier.Data4[2], boot_info->BootIdentifier.Data4[3], boot_info->BootIdentifier.Data4[4], boot_info->BootIdentifier.Data4[5], boot_info->BootIdentifier.Data4[6], boot_info->BootIdentifier.Data4[7]);
	
	return fnvr(serial);
}

uint64_t c_hwid::get_hwid()
{
	uint64_t ret = 0;
	ret ^= disk_serials();
	ret ^= boot_uuid();

	return ret;
}
