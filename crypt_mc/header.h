#pragma once
#include "context.h"

struct header_t
{
	uint8_t xor_key{};

	char username[32]{};
};

static _declspec(noinline) void parse_header(uintptr_t base)
{
#ifdef TESTBUILD
	strncpy_s(ctx.m_username, 32, "SenatorII", 32);
#else
	// create temp header
	header_t temp_header;

	// copy over header
	memcpy(&temp_header, (void*)base, sizeof(header_t));

	// unxor header
	for (size_t i = 1; i < sizeof(header_t); i++)
		*(uint8_t*)(uintptr_t(&temp_header) + i) ^= temp_header.xor_key;

	// process data from header
	memcpy(ctx.m_username, temp_header.username, 32);
#endif
}
