#pragma once

class c_hwid
{
	uint64_t m_hwid = 0;

	uint64_t disk_serials();
	uint64_t boot_uuid();
	uint64_t cpu_regs();
public:
	uint64_t get_hwid();
};