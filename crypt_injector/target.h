#pragma once
#include <string>
#include <vector>

enum arch_t {
	X86,
	X64
};

struct target_process {
	std::string display_name;
	std::string process_name;
	std::string dll_name;
	arch_t architecture;
	bool manual_map;
	int process_id;
};

class c_target {
	static std::vector<target_process> potential_targets;
	static int is_valid_target(const std::string& target_name);
public:
	static int find_target(target_process* target);
};