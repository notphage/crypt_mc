#pragma once

class c_hwid
{
	void error(LPTSTR lpszFunction);

	uint64_t m_hwid = 0;

	uint64_t disk_serials();
	uint64_t boot_uuid();
public:
	uint64_t get_hwid();
};