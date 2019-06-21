#include "context.h"

#include <tlhelp32.h> //snapshots

std::vector<target_process> c_target::potential_targets = {
	{ "Minecraft", "javaw.exe", "crypt_mc.dll", X64, true },
};

int c_target::is_valid_target(const std::string& target_name) {
	arch_t architecture = (arch_t)(sizeof(void*) == 8);

	for (size_t i = 0; i < potential_targets.size(); i++) {
		target_process& potential_target = potential_targets.at(i);
		if (potential_target.architecture == architecture && potential_target.process_name.compare(target_name) == 0) {
			return (int)i;
		}
	}

	return -1;
}

int c_target::find_target(target_process* target) {
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (hSnapshot == INVALID_HANDLE_VALUE)
		return -1;

	PROCESSENTRY32 entry = { NULL };
	entry.dwSize = sizeof(PROCESSENTRY32);

	if (!Process32First(hSnapshot, &entry)) {
		CloseHandle(hSnapshot);
		return -1;
	}

	do {
		int potential_target_id = is_valid_target(entry.szExeFile);
		if (potential_target_id != -1) {
			CloseHandle(hSnapshot);
			*target = potential_targets.at(potential_target_id);
			return entry.th32ProcessID;
		}
	} while (Process32Next(hSnapshot, &entry));

	CloseHandle(hSnapshot);
	return -1;
}