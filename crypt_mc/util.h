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
	static __forceinline float convert_legit_value(float step, float angle, float speed) {
		angle *= 0.15f;

		float steps = angle < 0.0f ? floorf(std::clamp(util::random(0.5f * speed, (0.5f * speed) + 0.5f), 1.f, 3.f) * angle * 5.0f) : ceilf(std::clamp(util::random(0.5f * speed, (0.5f * speed) + 0.5f), 1.f, 5.f) * angle * 5.0f);

		return steps * step * 0.15f;
	};

	static __forceinline void move_mouse(int cx, int cy)
	{
		POINT cursor_pos;
		GetCursorPos(&cursor_pos);

		SetCursorPos(cursor_pos.x + cx, cursor_pos.y + cy);
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

	namespace base64
	{
		static const std::string base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

		static bool is_base64(unsigned char c)
		{
			return (isalnum(c) || (c == '+') || (c == '/'));
		}

		static __forceinline std::string base64_encode(unsigned char const* bytes_to_encode, unsigned int in_len)
		{
			std::string ret;
			int i = 0;
			int j = 0;
			unsigned char char_array_3[3];
			unsigned char char_array_4[4];

			while (in_len--)
			{
				char_array_3[i++] = *(bytes_to_encode++);
				if (i == 3)
				{
					char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
					char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
					char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
					char_array_4[3] = char_array_3[2] & 0x3f;

					for (i = 0; (i < 4); i++)
						ret += base64_chars[char_array_4[i]];

					i = 0;
				}
			}

			if (i)
			{
				for (j = i; j < 3; j++)
					char_array_3[j] = '\0';

				char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
				char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
				char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
				char_array_4[3] = char_array_3[2] & 0x3f;

				for (j = 0; (j < i + 1); j++)
					ret += base64_chars[char_array_4[j]];

				while ((i++ < 3))
					ret += '=';

			}

			return ret;

		}

		static __forceinline std::string base64_decode(std::string const& encoded_string)
		{
			int in_len = encoded_string.size();
			int i = 0;
			int j = 0;
			int in_ = 0;
			unsigned char char_array_4[4], char_array_3[3];
			std::string ret;

			while (in_len-- && (encoded_string[in_] != '=') && is_base64(encoded_string[in_]))
			{
				char_array_4[i++] = encoded_string[in_];
				in_++;
				if (i == 4)
				{
					for (i = 0; i < 4; i++)
						char_array_4[i] = base64_chars.find(char_array_4[i]);

					char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
					char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
					char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

					for (i = 0; (i < 3); i++)
						ret += char_array_3[i];
					i = 0;
				}
			}

			if (i)
			{
				for (j = i; j < 4; j++)
					char_array_4[j] = 0;

				for (j = 0; j < 4; j++)
					char_array_4[j] = base64_chars.find(char_array_4[j]);

				char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
				char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
				char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

				for (j = 0; (j < i - 1); j++)
					ret += char_array_3[j];
			}

			return ret;
		}
	}

	static __forceinline HKEY open_reg_key(const char* key_path)
	{
		HKEY hk;
		LI_FN(RegCreateKeyExA).cached()(HKEY_CURRENT_USER, key_path, 0, nullptr, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, nullptr, &hk, nullptr);

		return hk;
	}

	static __forceinline void set_reg_key(HKEY key, const char* name, void* buffer, size_t len)
	{
		auto base64 = base64::base64_encode((const unsigned char*)buffer, strlen((const char*)buffer));

		LI_FN(RegSetValueExA).cached()(key, base64::base64_encode((const unsigned char*)name, strlen(name)).c_str(), 0, REG_SZ, (LPBYTE)base64.c_str(), (DWORD)base64.size());
		LI_FN(RegCloseKey).cached()(key);
	}

	static __forceinline void read_reg_key(HKEY key, const char* name, std::string& str)
	{
		char buffer[8192];
		DWORD len = sizeof(buffer);

		LI_FN(RegQueryValueExA).cached()(key, base64::base64_encode((const unsigned char*)name, strlen(name)).c_str(), nullptr, nullptr, (LPBYTE)&buffer, &len);
		LI_FN(RegCloseKey).cached()(key);

		str.append(base64::base64_decode(buffer));
	}

	static __forceinline void delete_reg_key(HKEY key, const char* name)
	{
		LI_FN(RegDeleteValueA).cached()(key, base64::base64_encode((const unsigned char*)name, strlen(name)).c_str());
		LI_FN(RegCloseKey).cached()(key);
	}

	static __forceinline void get_all_configs(std::vector<std::string>& out, const char* key_path)
	{
		auto config_key = open_reg_key(key_path);
		if (!config_key)
			return;

		DWORD num_sub_keys = 0;

		LI_FN(RegQueryInfoKeyA).cached()(config_key, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, &num_sub_keys, nullptr, nullptr, nullptr, nullptr);

		for (uint32_t i = 0; i < num_sub_keys; i++)
		{
			char buffer[64];
			DWORD len = sizeof(buffer);
			LI_FN(RegEnumValueA).cached()(config_key, i, buffer, &len, nullptr, nullptr, nullptr, nullptr);

			std::string config(base64::base64_decode(buffer));

			if (config.find(xors(".crypt")) == std::string::npos)
				continue;

			out.push_back(config.substr(0, config.find_last_of(".")));
		}

		LI_FN(RegCloseKey).cached()(config_key);
	}
}
