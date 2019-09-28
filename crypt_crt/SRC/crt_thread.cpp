#include "crt_time.h"
#include <Windows.h>

static int const epoch_start = -2147483648;

extern "C"
{
	int _Init_global_epoch = epoch_start;
	__declspec(thread) int _Init_thread_epoch = epoch_start;
}

extern "C" void _Thrd_sleep(const xtime* xt)
{
	xtime now;
	xtime_get(&now, 1);
	do
	{
		Sleep(_Xtime_diff_to_millis2(xt, &now));
		xtime_get(&now, 1);
	} while (now.sec < xt->sec || now.sec == xt->sec && now.nsec < xt->nsec);
}