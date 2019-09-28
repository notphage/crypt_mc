#include "crt_time.h"

#include <Windows.h>

#define NSEC_PER_SEC 1000000000L
#define NSEC_PER_MSEC 1000000L
#define NSEC_PER_USEC 1000L
#define MSEC_PER_SEC 1000

static void xtime_normalize(xtime* xt)
{
	while (xt->nsec < 0)
	{
		xt->sec -= 1;
		xt->nsec += NSEC_PER_SEC;
	}

	while (NSEC_PER_SEC <= xt->nsec)
	{
		xt->sec += 1;
		xt->nsec -= NSEC_PER_SEC;
	}
}

static xtime xtime_diff(const xtime* xt, const xtime* now)
{
	xtime diff = *xt;
	xtime_normalize(&diff);
	if (diff.nsec < now->nsec)
	{
		diff.sec -= now->sec + 1;
		diff.nsec += NSEC_PER_SEC - now->nsec;
	}
	else
	{
		diff.sec -= now->sec;
		diff.nsec -= now->nsec;
	}

	if (diff.sec < 0 || (diff.sec == 0 && diff.nsec <= 0))
	{
		diff.sec = 0;
		diff.nsec = 0;
	}

	return diff;
}

#define NSEC100_PER_SEC (NSEC_PER_SEC / 100)

#define EPOCH 0x19DB1DED53E8000i64

extern "C" long long _Xtime_get_ticks()
{
	static auto pGetSystemTimePrecisAsFileTime = (void (*)(FILETIME*))GetProcAddress(GetModuleHandleA("kernel32.dll"), "GetSystemTimePreciseAsFileTime");

	FILETIME ft;
	pGetSystemTimePrecisAsFileTime(&ft);
	return ((static_cast<long long>(ft.dwHighDateTime)) << 32) + static_cast<long long>(ft.dwLowDateTime) - EPOCH;
}

extern "C" long _Xtime_diff_to_millis2(const xtime* xt1, const xtime* xt2)
{
	xtime diff = xtime_diff(xt1, xt2);
	return static_cast<long>(diff.sec * MSEC_PER_SEC + (diff.nsec + NSEC_PER_MSEC - 1) / NSEC_PER_MSEC);
}

static void sys_get_time(xtime* xt)
{
	unsigned long long now = _Xtime_get_ticks();
	xt->sec  = static_cast<long long>(now / NSEC100_PER_SEC);
	xt->usec = static_cast<long>(now % NSEC100_PER_SEC) * 100;
}

int xtime_get(xtime* xt, int type)
{
	if (type != 1 || xt == 0)
		type = 0;
	else
		sys_get_time(xt);

	return type;
}

extern "C" long long __cdecl _Query_perf_counter()
{
	LARGE_INTEGER li;
	QueryPerformanceCounter(&li);
	return li.QuadPart;
}

extern "C" long long __cdecl _Query_perf_frequency()
{
	LARGE_INTEGER li;
	QueryPerformanceFrequency(&li);
	return li.QuadPart;
}
