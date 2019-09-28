#pragma once

struct xtime
{
	long long sec;
	unsigned long usec;
	long nsec;
};

extern "C" long _Xtime_diff_to_millis2(const xtime* xt1, const xtime* xt2);
extern int xtime_get(xtime* xt, int type);