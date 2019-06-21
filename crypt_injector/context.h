#pragma once

#define anti_rin( items )\
 static bool init = true; \
if ( init ) \
{ \
	items; \
	init = false;\
}

#include "hash.h"
#include "string.h"
#include "inject.h"
#include "target.h"

class c_context
{
public:
	c_syscall m_syscall;
	c_inject m_injector;
	c_target m_target;
};

extern c_context ctx;
