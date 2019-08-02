#pragma once

#include <Windows.h>
#include <new>
#include <random>
#include <string>
#include <iterator>
#include <vector>
#include <fstream>
#include <algorithm>
#include <malloc.h>

namespace util
{
	template <typename T>
	static __forceinline T random(T min, T max)
	{
		static std::random_device rd;
		static std::mt19937 gen(rd());
		std::uniform_real_distribution<> dis(min, max);

		return dis(gen);
	}

	static __forceinline int random_delay(int min, int max)
	{
		return 1000 / random(min, max);
	}

	static __forceinline std::string to_lower(const std::string& upper_case_string)
	{
		std::string lower_case_string(upper_case_string);
		std::transform(lower_case_string.begin(), lower_case_string.end(), lower_case_string.begin(), tolower);
		return lower_case_string;
	}

	static __forceinline std::string to_upper(const std::string& lower_case_string)
	{
		std::string upper_case_string(lower_case_string);
		std::transform(upper_case_string.begin(), upper_case_string.end(), upper_case_string.begin(), toupper);
		return upper_case_string;
	}

	static __forceinline bool is_number(const std::string& s)
	{
		std::string::const_iterator it = s.begin();
		while (it != s.end() && isdigit(*it))
			++it;
		return !s.empty() && it == s.end();
	}

	static __forceinline void make_murican(char* s)
	{
		std::string out{};
		for (size_t i = 0; i < strlen(s); ++i)
		{
			const char current_char = s[i];

			if ((ispunct(current_char) && current_char != '(' && current_char != ')') || isspace(current_char) || isalnum(current_char))
			{
				out.push_back(tolower(current_char));
				continue;
			}
		}

		if (!out.empty())
		{
			if (isspace(out[0]))
				out.erase(out.begin());

			strcpy(s, out.c_str());
		}
	}

	static __forceinline bool is_valid_text(const std::string& s)
	{
		return true;
		std::string::const_iterator it = s.begin();
		while (it != s.end() && (*it == '~' || *it == '_' || *it == '-' || isalnum(*it)))
			++it;
		return !s.empty() && it == s.end();
	}

	static __forceinline bool is_valid_text(const char& x)
	{
		return true;
		return x == '~' || x == '_' || x == '-' || isalnum(x);
	}

	static __forceinline bool exists_in_array(const std::vector<std::string>& vector, const std::string& item)
	{
		for (size_t i = 0; i < vector.size(); ++i)
		{
			if (vector[i].find(item) != std::string::npos)
				return true;
		}

		return false;
	}

	static __forceinline bool file_exists(const char* filename)
	{
		std::ifstream config(filename);
		return config.good();
	}

	static __forceinline bool valid_filename(const std::string& filename)
	{
		// http://hrt.rocks/u/pWGw.png
		if (filename.find("\\") != std::string::npos
		    || filename.find("/") != std::string::npos
		    || filename.find("|") != std::string::npos
		    || filename.find("\"") != std::string::npos
		    || filename.find(":") != std::string::npos
		    || filename.find("?") != std::string::npos
		    || filename.find("*") != std::string::npos
		    || filename.find("<") != std::string::npos
		    || filename.find(">") != std::string::npos
		)
			return false;

		return true;
	}

	static __forceinline HKEY open_reg_key(const char* key_path)
	{
		HKEY hk;
		LI_FN(RegCreateKeyExA).cached()(HKEY_CURRENT_USER, key_path, 0, nullptr, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, nullptr, &hk, nullptr);

		return hk;
	}

	static __forceinline void set_reg_key(HKEY key, const char* name, void* buffer, size_t len)
	{
		LI_FN(RegSetValueExA).cached()(key, name, 0, REG_SZ, (LPBYTE)buffer, (DWORD)len);
		LI_FN(RegCloseKey).cached()(key);
	}

	static __forceinline void read_reg_key(HKEY key, const char* name, std::string& str)
	{
		char buffer[4096];
		DWORD len = sizeof(buffer);

		LI_FN(RegQueryValueExA).cached()(key, name, nullptr, nullptr, (LPBYTE)&buffer, &len);
		LI_FN(RegCloseKey).cached()(key);

		str.append(buffer);
	}

	static __forceinline void delete_reg_key(HKEY key, const char* name)
	{
		LI_FN(RegDeleteValueA).cached()(key, name);
		LI_FN(RegCloseKey).cached()(key);
	}

	static __forceinline void get_all_configs(std::vector<std::string>& out, const char* key_path)
	{
		auto config_key = open_reg_key(key_path);
		if (!config_key)
			return;

		DWORD num_sub_keys = 0;

		LI_FN(RegQueryInfoKeyA).cached()(config_key, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, &num_sub_keys, nullptr, nullptr, nullptr, nullptr);

		for (auto i = 0; i < num_sub_keys; i++)
		{
			char buffer[64];
			DWORD len = sizeof(buffer);
			LI_FN(RegEnumValueA).cached()(config_key, i, buffer, &len, nullptr, nullptr, nullptr, nullptr);

			std::string config(buffer);

			if (config.find(xors(".crypt")) == std::string::npos)
				continue;

			out.push_back(config.substr(0, config.find_last_of(".")));
		}

		LI_FN(RegCloseKey).cached()(config_key);
	}
}
