#pragma once
#pragma warning(disable : 4996)
#pragma warning(disable : 4018)
#include <Windows.h>
#include <new>
#include <string>
#include <iterator>
#include <vector>
#include <sstream>
#include <intrin.h>

namespace memory
{
	enum dll_sections_t
	{
		SECTION_TEXT, //.text
		SECTION_RDATA, //.rdata
		SECTION_DATA, //.data
		SECTION_RSRC, //.rsrc
		SECTION_RELOC, //.reloc
		SECTION_MAX
	};

	// c_stack
	// stack manager/ helper class	

	// msvc compiletime fix
#define get_ebp  ( void* ) ( ( uintptr_t ) _AddressOfReturnAddress() - sizeof( uintptr_t ) )

	class c_stack
	{
	private:
		void* base = nullptr;
	public:
		c_stack(void* base)
		{
			this->base = base;
		}

		void previous(unsigned int frames = 1)
		{
			for (; frames < 0; --frames)
			{
				base = *(void**)base;
			}
		}

		template <typename t>
		t get_local(uintptr_t offset)
		{
			return (t)((uintptr_t)base - offset);
		}

		template <typename t>
		t get_arg(uintptr_t offset)
		{
			(t)get_retaddr() + offset;
		}

		uintptr_t get_retaddr()
		{
			return (uintptr_t)base + sizeof(uintptr_t);
		}
	};

	class c_data_memory
	{
		struct import_data_t
		{
			uintptr_t start{};
			uintptr_t size{};

			uint8_t map[0x4000]{};

			uintptr_t get_memory_location(size_t desired_size)
			{
				if (!start || !size)
					return 0;

				for (size_t i = 0; i < size; i++)
				{
					// already used
					if (map[i] != 0)
						continue;

					// doesnt fit
					if (i + desired_size > size)
						continue;

					// mask the map
					memset(&map[i], 1, desired_size);

					return start + i;
				}

				return 0;
			}

			size_t unused_size()
			{
				if (!start || !size)
					return 0;

				for (size_t i = 0; i < size; i++)
				{
					if (map[i] != 0)
						continue;

					return size - i;
				}

				return 0;
			}
		};

		HMODULE m_base{};
		import_data_t m_pibn{};
	public:
		bool scan(const char* name)
		{
			m_base = GetModuleHandleA(name);

			if (!m_base)
				return false;

			IMAGE_DOS_HEADER* dos_header = (IMAGE_DOS_HEADER*)m_base;
			IMAGE_NT_HEADERS* nt_header = (IMAGE_NT_HEADERS*)((uintptr_t)m_base + dos_header->e_lfanew);
			IMAGE_IMPORT_DESCRIPTOR* import_directory = (IMAGE_IMPORT_DESCRIPTOR*)((uintptr_t)m_base + nt_header->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);

			uintptr_t module_start = (uintptr_t)m_base;
			uintptr_t module_end = (uintptr_t)m_base + nt_header->OptionalHeader.SizeOfImage;

			uintptr_t pibn_start = UINT_MAX;
			uintptr_t pibn_end = 0;

			while (import_directory->Characteristics)
			{
				//get thunk data
				IMAGE_THUNK_DATA* original_first_thunk = (IMAGE_THUNK_DATA*)(module_start + import_directory->OriginalFirstThunk);
				IMAGE_THUNK_DATA* first_thunk = (IMAGE_THUNK_DATA*)(module_start + import_directory->FirstThunk);

				//bb got dat func in da thunk
				while (original_first_thunk->u1.AddressOfData)
				{
					if (!(original_first_thunk->u1.Ordinal & IMAGE_ORDINAL_FLAG))
					{
						IMAGE_IMPORT_BY_NAME* import_by_name = (IMAGE_IMPORT_BY_NAME*)(module_start + original_first_thunk->u1.AddressOfData);
						uintptr_t name_address = (uintptr_t)import_by_name->Name;

						if (name_address < pibn_start)
							pibn_start = name_address;

						if (name_address > pibn_end)
							pibn_end = name_address;
					}

					original_first_thunk++;
					first_thunk++;
				}

				import_directory++;
			}

			if (pibn_start < module_start || pibn_start > module_end)
				return false;

			if (pibn_end < module_start || pibn_end > module_end)
				return false;

			if (pibn_start >= pibn_end)
				return false;

			m_pibn.start = pibn_start;
			m_pibn.size = std::min<uintptr_t>(pibn_end - pibn_start, 0x4000);
			memset(m_pibn.map, 0, 0x4000);

			return m_pibn.size > 0x100;
		}

		import_data_t* pibn()
		{
			return &m_pibn;
		}
	};

	class c_vmt
	{
		bool m_is_hooked{};
		bool m_replaced_table{};

		uintptr_t* m_table{};
		uintptr_t* m_original{};

		uintptr_t* m_custom{};

		c_data_memory* m_data_memory{};

		bool is_code_pointer(uintptr_t p)
		{
			constexpr const DWORD protect_flags = PAGE_EXECUTE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY;

			MEMORY_BASIC_INFORMATION out;
			LI_FN(VirtualQuery).cached()((void*)p, &out, sizeof out);

			return out.Type
			       && !(out.Protect & (PAGE_GUARD | PAGE_NOACCESS))
			       && out.Protect & protect_flags;
		}

	public:
		c_vmt() { }

		c_vmt(void* table)
		{
			m_table = reinterpret_cast<uintptr_t*>(table);
			m_original = *reinterpret_cast<uintptr_t**>(m_table);
		}

		~c_vmt()
		{
			restore();
		}

		bool is_hooked()
		{
			return m_is_hooked;
		}

		// this is only used by netchannel hook which is a dynamic object and runtime
		void set(void* table, bool should_replace_table = true)
		{
			if (!table)
				return;

			m_replaced_table = should_replace_table;

			m_table = reinterpret_cast<uintptr_t*>(table);
			m_original = *reinterpret_cast<uintptr_t**>(m_table);

			int vfunc_count = count();

			delete[] m_custom;
			m_custom = new uintptr_t[vfunc_count + 1];

			for (int i = -1; i < vfunc_count; ++i)
				m_custom[i + 1] = m_original[i];

			if (should_replace_table)
				replace_table(m_table);

			m_is_hooked = true;
		}

		void replace_table(uintptr_t* table) const
		{
			*table = uintptr_t(&m_custom[1]);
		}

		void restore_table(uintptr_t* table) const
		{
			*table = uintptr_t(m_original);
		}

		void set(void* table, c_data_memory* data_memory, bool should_replace_table = true)
		{
			if (!table)
				return;

			m_replaced_table = should_replace_table;

			m_table = reinterpret_cast<uintptr_t*>(table);
			m_original = *reinterpret_cast<uintptr_t**>(m_table);
			m_data_memory = data_memory;

			int vfunc_count = count();

			size_t needed_size = (vfunc_count + 1) * sizeof(uintptr_t);
			uintptr_t address = m_data_memory->pibn()->get_memory_location(needed_size);

			if (!address)
				return;

			m_custom = (uintptr_t*)address;

			DWORD orig_prot;
			VirtualProtect((void*)address, needed_size, PAGE_READWRITE, &orig_prot);

			for (int i = -1; i < vfunc_count; ++i)
				m_custom[i + 1] = m_original[i];

			//VirtualProtect( (void*)address, needed_size, orig_prot, &orig_prot );

			if (should_replace_table)
				replace_table(m_table);

			m_is_hooked = true;
		}

		int count()
		{
			if (!m_original)
				return 0;

			int vfunc_count{};

			while (m_original[vfunc_count] && is_code_pointer(m_original[vfunc_count]))
				vfunc_count++;

			return vfunc_count;
		}

		template <typename T = uintptr_t>
		T get_function(void* func, int index)
		{
			if (!m_custom)
				return ((T)(0));

			if (m_data_memory)
			{
				uintptr_t jmp_patch = m_data_memory->pibn()->get_memory_location(5);

				if (!jmp_patch)
					return ((T)(0));

				DWORD orig_prot;
				VirtualProtect((void*)jmp_patch, 5, PAGE_EXECUTE_READWRITE, &orig_prot);

				*(uint8_t*)(jmp_patch + 0) = 0xE9;
				*(uintptr_t*)(jmp_patch + 1) = uintptr_t(func) - jmp_patch - 5;

				m_custom[index + 1] = jmp_patch;
			}
			else
			{
				m_custom[index + 1] = reinterpret_cast<uintptr_t>(func);
			}

			return ((T)(m_original[index]));
		}

		template <typename T = uintptr_t>
		T get_original(int index)
		{
			return ((T)(m_original[index]));
		}

		void unhook(int index)
		{
			if (!m_custom)
				return;

			DWORD orig_prot;
			VirtualProtect((void*)&m_custom[index + 1], sizeof(uintptr_t), PAGE_READWRITE, &orig_prot);

			m_custom[index + 1] = m_original[index];

			VirtualProtect((void*)&m_custom[index + 1], sizeof(uintptr_t), orig_prot, &orig_prot);
		}

		void restore() const
		{
			if (m_replaced_table)
				*m_table = uintptr_t(m_original);
		}
	};

	template <typename fn>
	__forceinline fn call_vfunc(void* classbase, int index)
	{
		return (fn)(*(uintptr_t**)classbase)[index];
	}
#pragma optimize( "", off )
	template <typename fn>
	__declspec(noinline) fn get_vfunc(void* classbase, int index)
	{
		if (classbase && *(uintptr_t**)classbase && (*(uintptr_t**)classbase)[index])
			return call_vfunc<fn>(classbase, index);

#ifdef TESTBUILD
		printf("oopsie woopsie we made a fucky wucky ;w;w %i\n", index);
#endif
		return nullptr;
	}
#pragma optimize( "", on )

	template <typename t = uintptr_t>
	static t pattern_search(
		// module to search within
		const HMODULE& module,

		// pattern of bytes to search for
		const char* pattern,

		// offset in bytes from address obtained by pattern searching
		const int& offset = 0,

		// search region
		const uintptr_t& region_start = std::numeric_limits<uintptr_t>::max(),
		const uintptr_t& region_end = 0
	)
	{
		auto get_pattern = [](const char* raw_pattern) -> std::vector<uintptr_t>
				{
					std::vector<uintptr_t> pattern;
					char* start = (char*)raw_pattern;
					char* end = (char*)(raw_pattern) + strlen(raw_pattern);

					for (char* byte = start; byte < end; ++byte)
					{
						// handle ida sttyle signatures, like a1 ? ? ? ? or a1 ?? ??
						if (*byte == '?')
						{
							// jump past ? in string, see if we land on another ?
							if (*(++byte) == '?')
								byte++;

							pattern.push_back(std::numeric_limits<uintptr_t>::max());
						}
						else
						{
							// we have a valid byte to push back to our new pattern
							pattern.push_back(strtoul(byte, &byte, 16));
						}
					}

					return pattern;
				};

		auto get_pattern_fast = [](uint8_t* bytes, uint8_t* mask, const char* raw_pattern) -> size_t
				{
					std::vector<uintptr_t> pattern;
					char* start = (char*)raw_pattern;
					char* end = (char*)(raw_pattern) + strlen(raw_pattern);

					size_t counter = 0;

					for (char* byte = start; byte < end; ++byte)
					{
						// handle ida sttyle signatures, like a1 ? ? ? ? or a1 ?? ??
						if (*byte == '?')
						{
							// jump past ? in string, see if we land on another ?
							if (*(++byte) == '?')
								byte++;

							bytes[counter] = 0;
							mask[counter] = 0;
							counter++;
						}
						else
						{
							// we have a valid byte to push back to our new pattern
							bytes[counter] = (uint8_t)(strtoul(byte, &byte, 16));
							mask[counter] = 1;
							counter++;
						}
					}

					return counter;
				};

		PIMAGE_DOS_HEADER dos_header = reinterpret_cast<PIMAGE_DOS_HEADER>(module);
		PIMAGE_NT_HEADERS nt_headers{};

		if (!dos_header || dos_header->e_magic != IMAGE_DOS_SIGNATURE)
			return 0;

		nt_headers = reinterpret_cast<PIMAGE_NT_HEADERS>((uintptr_t)dos_header + dos_header->e_lfanew);

		if (!nt_headers || nt_headers->Signature != IMAGE_NT_SIGNATURE)
			return 0;

		const auto bin = reinterpret_cast<uint8_t*>(module);
		const uintptr_t start = region_start != std::numeric_limits<uintptr_t>::max() ? region_start : 0;
		const uintptr_t end = region_end > 0 ? region_end : nt_headers->OptionalHeader.SizeOfImage;

		// before u cry, check this
		// > pattern scan took: 312 ms
		// > pattern scan took: 5234 ms
		// also please reconsider life if u want to use a signature over 512 bytes
		static uint8_t bytes[512]{};
		static uint8_t mask[512]{};
		size_t size = get_pattern_fast(bytes, mask, pattern);

		//const auto bytes = get_pattern( pattern );

		for (uintptr_t address = start; address < end - size; ++address)
		{
			bool found_pattern = true;

			for (uintptr_t token = 0; token < size; ++token)
			{
				if (bytes[token] != bin[address + token] && mask[token] == 1)
				{
					// either invalid portion of signature or the current bytes don't match
					// break and invalidate return value for now
					found_pattern = false;
					break;
				}
			}

			if (found_pattern)
			{
				// success, return address
				return (t)(&bin[address] + offset);
			}
		}

#ifdef TESTBUILD
		printf("OUTDATED SIG: %s \n", pattern);
#endif

		// didn't find pattern
		return (t)0;
	}
}
