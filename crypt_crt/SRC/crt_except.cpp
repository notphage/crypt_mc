#include <intrin.h>
#include <ehdata.h>

//
// If any exception related function is marked as unresolved, simply
// declare it in a similar fashion below. You may also implement
// your own exception-handling code here.
//

namespace std
{
	void __cdecl _Xlength_error(const char*)
	{
		__debugbreak();
	}

	void __cdecl _Xbad_function_call(void)
	{
		__debugbreak();
	}

	void __cdecl _Xout_of_range(const char*)
	{
		__debugbreak();
	}
}

extern "C" __declspec(noreturn) void __cdecl _invalid_parameter_noinfo_noreturn()
{
	__debugbreak();
}

extern "C" __declspec(noreturn) void __stdcall _CxxThrowException(void*, _ThrowInfo*)
{
	__debugbreak();
}

extern "C" int __cdecl _purecall()
{
	ExitProcess(0);
}

//
// Below lays an interesting case, this one is more alike the SEH handlers.
// Either way, we can do this ghetto thing and call it a day.
// 
extern "C" EXCEPTION_DISPOSITION __cdecl __CxxFrameHandler3(EHExceptionRecord*, EHRegistrationNode*, void*, DispatcherContext*)
{
	__debugbreak();
	return ExceptionContinueExecution;
}