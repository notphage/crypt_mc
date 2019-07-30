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

	static __forceinline void get_all_files(std::vector<std::string>& out, const std::string& directory)
	{
		//HANDLE dir;
		//WIN32_FIND_DATA file_data;
		//
		//if ((dir = FindFirstFile((directory + "/*").c_str(), &file_data)) == INVALID_HANDLE_VALUE)
		//	return;
		//
		//do
		//{
		//	const std::string file_name = file_data.cFileName;
		//	std::string full_file_name = directory + "/" + file_name;
		//	const bool is_directory = (file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
		//
		//	if (file_name[0] == '.' || !strstr(file_name.c_str(), xors(".cfg")))
		//		continue;
		//
		//	if (is_directory)
		//		continue;
		//
		//	if (file_name.find(xors("default.cfg")) != std::string::npos || file_name.find(xors("skins.cfg")) != std::string::npos)
		//		continue;
		//
		//	// cull .// and .cfg
		//	out.push_back((full_file_name.substr(0, full_file_name.find_last_of(".")).substr(3)));
		//}
		//while (FindNextFile(dir, &file_data));
		//
		//FindClose(dir);
	}

	static __forceinline void simple_save(uint32_t name, const void* setting, size_t size, const char* filename)
	{
		//char* buffer = (char*)_alloca(size * 2 + 1);
		//uint8_t* data = (uint8_t*)setting;
		//
		//for (size_t i = 0; i < size; i++)
		//	sprintf(&buffer[2 * i], xors("%02X"), data[i]);
		//
		//// printf( "writing to %s ", filename );
		//WritePrivateProfileStringA("_", std::to_string(name).c_str(), buffer, filename);
	}

	static __forceinline void simple_load(uint32_t name, void* setting, unsigned long size, const char* filename)
	{
		//char* buffer = (char*)_alloca((size_t)size * 2 + 1);
		//uint8_t* data = (uint8_t*)setting;
		//
		//GetPrivateProfileStringA("_", std::to_string(name).c_str(), "", buffer, size * 2 + 1, filename);
		//
		//if (*buffer == 0)
		//	return;
		//for (size_t i = 0; i < size; i++)
		//{
		//	unsigned temp;
		//	sscanf(&buffer[2 * i], xors("%02X"), &temp);
		//	data[i] = temp;
		//}
	}

	static __forceinline void create_file(const char* filename, uint32_t creation_flags = CREATE_ALWAYS, uint32_t access_flags = GENERIC_READ | GENERIC_WRITE)
	{
		//const handle_t file = CreateFileA(filename, access_flags, 0, 0, creation_flags, FILE_ATTRIBUTE_NORMAL, 0);
		//
		//if (file != INVALID_HANDLE_VALUE)
		//	CloseHandle(file);
	}
}
