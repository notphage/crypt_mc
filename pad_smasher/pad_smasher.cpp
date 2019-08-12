#include <Windows.h>

#include <string>
#include <cinttypes>
#include <vector>
#include <fstream>
#include <sstream>
#include <regex>
#include <conio.h>
#include <cstdint>
#include <random>

struct func_t
{
	std::string m_name;
	uintptr_t m_raw_addr = 0;
	uintptr_t m_rva_addr = 0;
	uintptr_t m_pad_start = 0;
	size_t m_pad_len = 0;
};

std::vector<func_t> functions;

std::pair<uint8_t*, size_t> load_file(const char* name)
{
	// we load ntdll from disk, as u cant edit this ( causes bsod unless patchguard is disabled )
	// could even compare ntdll in process to disk, to find potentially malicious users

	char path[MAX_PATH]{};
	GetCurrentDirectoryA(MAX_PATH, path);

	std::string dll_path(path);
	dll_path += name;

	FILE* file = nullptr;
	if (fopen_s(&file, dll_path.c_str(), "rb") != 0)
		return {};

	fseek(file, 0, SEEK_END);
	const size_t dll_size = ftell(file);
	rewind(file);

	const auto dll = new uint8_t[dll_size];
	fread(dll, dll_size, 1, file);
	fclose(file);

	return { dll, dll_size };
}

uintptr_t rva_to_offset(uintptr_t rva, PIMAGE_SECTION_HEADER section_raw, size_t count)
{
	size_t i = 0;
	for (i = 0; i < count; ++i)
	{
		const auto section_begin_rva = section_raw[i].VirtualAddress;
		const auto section_end_rva = section_begin_rva + section_raw[i].Misc.VirtualSize;
		if (section_begin_rva <= rva && rva <= section_end_rva)
			break;
	}

	return rva - section_raw[i].VirtualAddress + section_raw[i].PointerToRawData;
}

void parse_map_file(uintptr_t file_ptr)
{
	auto dos = (PIMAGE_DOS_HEADER)file_ptr;
	auto nt = (PIMAGE_NT_HEADERS)(file_ptr + dos->e_lfanew);
	auto section = (PIMAGE_SECTION_HEADER)(file_ptr + dos->e_lfanew + sizeof(IMAGE_NT_HEADERS));
	auto sections_num = nt->FileHeader.NumberOfSections;
	uintptr_t image_base = nt->OptionalHeader.ImageBase;

	char path[MAX_PATH]{};
	GetCurrentDirectoryA(MAX_PATH, path);

	std::string map_path(path);
	map_path += "\\crypt_injector.map";
	
	FILE* file = nullptr;
	if (fopen_s(&file, map_path.c_str(), "rb") != 0)
		return;

	fseek(file, 0, SEEK_END);
	const size_t dll_size = ftell(file);
	rewind(file);

	auto dll = new char[dll_size];
	fread(dll, dll_size, 1, file);
	fclose(file);

	std::stringstream st(dll);

	std::string line;
	line.reserve(4096);

	bool symbols_found = false;
	auto skip_count = 0;
	while (std::getline(st, line))
	{
		if ((!symbols_found && line.find("Publics by Value") == std::string::npos) || line.length() <= 21)
			continue;
		
		symbols_found = true;

		if (skip_count < 2)
		{
			skip_count++;
			continue;
		}

		func_t function{};
		
		std::string symbol_str = line.substr(21);
		symbol_str = symbol_str.substr(0, symbol_str.find_first_of(' '));
		function.m_name = symbol_str;

		//if (function.m_name.at(0) == '_')
		//	continue;

		std::string offset_str = line.substr(21 + symbol_str.length());
		offset_str = offset_str.substr(offset_str.find_first_not_of(' '));
		offset_str = offset_str.substr(0, offset_str.find_first_of(' '));
		function.m_rva_addr = std::strtoull(offset_str.c_str(), nullptr, 16);

		if (function.m_rva_addr < image_base + 0x1000)
			continue;

		function.m_raw_addr = file_ptr + rva_to_offset(function.m_rva_addr - image_base, section, sections_num);

		auto func_addr = function.m_raw_addr;
		for (auto ins = *reinterpret_cast<uint8_t*>(func_addr - 1); ins == 0xCC || ins == 0x90;)
		{
			ins = *reinterpret_cast<uint8_t*>(func_addr - 1);
			func_addr -= 1;
			function.m_pad_len++;
		}

		if (function.m_pad_len > 0)
		{
			function.m_pad_len -= 1;
			function.m_pad_start = function.m_raw_addr - function.m_pad_len;
		}

		functions.push_back(function);
		
		printf("symbol: %s rva: 0x%" PRIx64 " raw: 0x%" PRIx64 " pad: 0x%" PRIx64 " pad len: %llu\n", symbol_str.c_str(), function.m_rva_addr, function.m_raw_addr, function.m_pad_start, function.m_pad_len);
	}
}

int main()
{
	const auto pair = load_file("\\crypt_injector.exe");
	auto dll = pair.first;
	auto size = pair.second;
	parse_map_file(reinterpret_cast<uintptr_t>(dll));

	std::random_device rd;
	std::mt19937 e2(rd());
	const std::uniform_int_distribution<uint32_t> switch_dist(0, 2);
	const std::uniform_int_distribution<uint32_t> sub_esp_dist(0, 174);
	const std::uniform_int_distribution<uint32_t> push_reg_dist(0x50, 0x57);
	const std::uniform_int_distribution<uint32_t> pop_reg_dist(0x58, 0x5f);

	for (auto function_iter = functions.begin(); function_iter < functions.end(); ++function_iter)
	{
		auto function = *function_iter;
		
		if (function.m_pad_len < 1)
			continue;

		auto next_function = *std::next(function_iter);
		auto bytes_left = function.m_pad_len;
		auto current_ins = reinterpret_cast<uint8_t*>(function.m_pad_start + function.m_pad_len - bytes_left);

		if (*(current_ins - 1) == 0xC3) // nop sled
		{
			*(current_ins + function.m_pad_len - 1) = 0xC3; // ret
			
			for (size_t i = 0; i < function.m_pad_len; i++)
				*(current_ins + i - 1) = 0x90; // nop

			if (function.m_pad_len >= 9)
			{
				*(current_ins + function.m_pad_len - 8) = 0x48; // xor rax, rax
				*(current_ins + function.m_pad_len - 7) = 0x31;
				*(current_ins + function.m_pad_len - 6) = 0xd0;
				*(current_ins + function.m_pad_len - 5) = 0x74; // jz 0x1
				*(current_ins + function.m_pad_len - 4) = 0xfc;
				*(current_ins + function.m_pad_len - 3) = 0xe9; // jmp
				*(current_ins + function.m_pad_len - 2) = 0x58; // pop rax
			}

			printf("%s: wrote nop sled to 0x%" PRIx64 "\n", function.m_name.c_str(), reinterpret_cast<uintptr_t>(current_ins));
		}
		//else // junk instructions
		//{
		//	do
		//	{
		//		current_ins = reinterpret_cast<uint8_t*>(function.m_pad_start + function.m_pad_len - bytes_left);
		//
		//		if (bytes_left == 1)
		//		{
		//			*current_ins = 0xe9; // jmp
		//
		//			printf("%s: wrote jmp to 0x%" PRIx64 "\n", function.m_name.c_str(), reinterpret_cast<uintptr_t>(current_ins));
		//			break;
		//		}
		//
		//		if (bytes_left >= 12) // movabs rax, addr
		//		{                     // jmp rax
		//			*(current_ins + 0) = 0x48;
		//			*(current_ins + 1) = 0xb8;
		//			*(current_ins + 2) = ((uint8_t*)& next_function.m_rva_addr)[0];
		//			*(current_ins + 3) = ((uint8_t*)& next_function.m_rva_addr)[1];
		//			*(current_ins + 4) = ((uint8_t*)& next_function.m_rva_addr)[2];
		//			*(current_ins + 5) = ((uint8_t*)& next_function.m_rva_addr)[3];
		//			*(current_ins + 6) = ((uint8_t*)& next_function.m_rva_addr)[4];
		//			*(current_ins + 7) = ((uint8_t*)& next_function.m_rva_addr)[5];
		//			*(current_ins + 8) = ((uint8_t*)& next_function.m_rva_addr)[6];
		//			*(current_ins + 9) = ((uint8_t*)& next_function.m_rva_addr)[7];
		//			*(current_ins + 10) = 0xff;
		//			*(current_ins + 11) = 0xe0;
		//
		//			bytes_left -= 12;
		//
		//			printf("%s: wrote movabs rax, addr jmp rax to 0x%" PRIx64 "\n", function.m_name.c_str(), reinterpret_cast<uintptr_t>(current_ins));
		//			continue;
		//		}
		//
		//		switch (switch_dist(e2))
		//		{
		//		case 0: // sub esp, rand(0, 127)
		//		{
		//			if (bytes_left < 5)
		//				continue;
		//
		//			*(current_ins + 0) = 0x48;
		//			*(current_ins + 1) = 0x83;
		//			*(current_ins + 2) = 0xec;
		//			*(current_ins + 3) = static_cast<uint8_t>(sub_esp_dist(e2));
		//
		//			bytes_left -= 4;
		//
		//			printf("%s: wrote sub esp, rand(0, 127) to 0x%" PRIx64 "\n", function.m_name.c_str(), reinterpret_cast<uintptr_t>(current_ins));
		//			break;
		//		}
		//
		//		case 1: // push reg
		//		{
		//			*current_ins = static_cast<uint8_t>(push_reg_dist(e2));
		//
		//			bytes_left -= 1;
		//
		//			printf("%s: wrote push reg to 0x%" PRIx64 "\n", function.m_name.c_str(), reinterpret_cast<uintptr_t>(current_ins));
		//			break;
		//		}
		//
		//		case 2: // pop reg
		//		{
		//			*current_ins = static_cast<uint8_t>(pop_reg_dist(e2));
		//
		//			bytes_left -= 1;
		//
		//			printf("%s: wrote pop reg to 0x%" PRIx64 "\n", function.m_name.c_str(), reinterpret_cast<uintptr_t>(current_ins));
		//			break;
		//		}
		//
		//		default:
		//			break;
		//		}
		//	} while (bytes_left > 0);
		//}
	}

	char path[MAX_PATH]{};
	GetCurrentDirectoryA(MAX_PATH, path);

	std::string map_path(path);
	map_path += "\\crypt_injector.padsmashed.exe";

	std::ofstream fp;
	fp.open(map_path, std::ios::out | std::ios::binary);
	fp.write(reinterpret_cast<const char*>(dll), size);
	fp.close();

	_getch();

	return 0;
	
}
