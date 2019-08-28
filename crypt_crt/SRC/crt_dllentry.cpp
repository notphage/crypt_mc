#include "crt_init.h"

//
// The user defined entry-point for our image.
// This will get called by _DllMainCRTStartup once all the constructors
// have been invoked.
//
int __stdcall DllMain(void* image, int reason, void* reserved);

//
// The actual entry-point for our image, this is what IMAGE_NT_HEADERS
// will display as EntryPoint (as well as the export table).
//
extern "C" int __stdcall _DllMainCRTStartup(void* image, int reason, void* reserved)
{
	// We are entering the process, setup the atexit table and
	// invoke all the constructors.
	if (reason == DLL_PROCESS_ATTACH)
	{
		_atexit_init();
		_initterm(__xc_a, __xc_z);
	}

	// NOTE: You could pass a loader stub through 'reserved'.
	int result = DllMain(image, reason, reserved);

	// We are exiting the process, call all destructors.
	if (reason == DLL_PROCESS_DETACH)
		_DoExit();

	return result;
}