#pragma once

#pragma warning( disable : 4100 ) //unreferenced formal parameter
#pragma warning( disable : 4101 ) //unreferenced local variable
#pragma warning( disable : 4189 ) //local variable is initialized but not referenced
#pragma warning( disable : 4389 ) //signed/unsigned mismatch

// fuck exceptions
#define _HAS_EXCEPTIONS 0
#define NOMINMAX

#include <jni.h>
#include <jvmti.h>

#include "sdk.h"
#include "hash.h"
#include "string.h"
#include "util.h"
#include "gui.h"
#include "math.h"
#include "feature.h"
