#pragma once

#define EXPAND( x,y )x##y

#define BEGIN___( MARKER_BEGIN )MARKER_BEGIN:

#define DEL_END___( MARKER_BEGIN, MARKER_END )__asm { __asm PUSH MARKER_END __asm PUSH MARKER_BEGIN __asm CALL erase } \
MARKER_END:

#define BEGIN__( x, y ) BEGIN___( EXPAND2( MARKER_BEGIN_, x, y ) )
#define END__( x, y ) DEL_END___( EXPAND2( MARKER_BEGIN_, x, y ), EXPAND2( MARKER_END_, x, y ) )

#define DELETE_START( x ) BEGIN___( EXPAND( MARKER_BEGIN_, x ) )
#define DELETE_END( x ) DEL_END___( EXPAND( MARKER_BEGIN_, x ), EXPAND( MARKER_END_, x ) )

//the main purpose of this fuction is to generate assembly that will confuse reverse engineers.
//lets throw some game related values in here
//such as 979, 980
//floats: 0.22, 1.1, 4096
void generate_random_aseembly(void* out, size_t size)
{
	//some patterns the code will use
	/*
			0:  55                      push   ebp
			1:  89 e5                   mov    ebp,esp

			;some cdecl call
			3:  50                      push   eax
			4:  53                      push   ebx
			5:  e8 eb be ad de          call   deadbef5 <_main+0xdeadbef5>
			a:  83 c4 08                add    esp,0x8

			d:  31 c0                   xor    eax,eax
			f:  b8 00 00 00 00          mov    eax,0x0

			14: 05 de c0 ad de          add    eax,0xdeadc0de

			19: 3d 39 05 00 00          cmp    eax,0x539
			1e: 83 f8 00                cmp    eax,0x0

			21: 0f 84 eb be ad de       je     deadbf12 <_main+0xdeadbf12>
			27: e9 eb be ad de          jmp    deadbf17 <_main+0xdeadbf17>
			2c: 0f 84 eb be ad de       je     deadbf1d <_main+0xdeadbf1d>

			33: 5d                      pop    ebp
	*/
	//TODO: refactor, not make this a std string
	auto generate_cdecl_call = [&](int arg_size)-> std::string
			{
				byte push_32_reg[] = {0x50,0x53,0x51,0x52};
				/*
					;32 bit registers
					0:  50                      push   eax
					1:  53                      push   ebx
					2:  51                      push   ecx
					3:  52                      push   edx
		
					;16 bit registers
					0:  66 50                   push   ax
					2:  66 53                   push   bx
					4:  66 51                   push   cx
					6:  66 52                   push   dx
		
					;custom number
					0:  68 39 05 00 00          push   0x539
		
					;ptr memes
					0:  ff b4 24 ff 00 00 00    push   DWORD PTR [esp+0xff]
		
					;smaller ptr memes
					0:  ff 74 24 0f             push   DWORD PTR [esp+0xf]
		
					0:  81 c4 ef be ad de       add    esp,0xdeadbeef
				*/

				std::string result;
				if (arg_size % 4 == 0)
				{
					for (size_t i = 0; i < arg_size; i++)
					{
						if (i < arg_size / 2) // first half
						{
							result += push_32_reg[i % 4];
						}
						else
						{
							if (i % 2 == 0)
							{
								result += "\x68";

								byte random_byte_1 = rand() % 0xFF;
								byte random_byte_2 = rand() % 0xFF;
								byte random_byte_3 = rand() % 0xFF;
								byte random_byte_4 = rand() % 0xFF;

								result += random_byte_1;
								result += random_byte_2;
								result += random_byte_3;
								result += random_byte_4;
							}
							else
							{
								result += "\xFF\xB4\x24";

								byte random_byte_1 = rand() % 0xFF;
								byte random_byte_2 = rand() % 0xFF;
								byte random_byte_3 = rand() % 0xFF;
								byte random_byte_4 = rand() % 0xFF;

								result += random_byte_1;
								result += random_byte_2;
								result += random_byte_3;
								result += random_byte_4;
							}
						}
					}
				}

				//actual call
				byte random_byte_1 = rand() % 0xFF;
				byte random_byte_2 = rand() % 0xFF;
				byte random_byte_3 = rand() % 0xFF;
				byte random_byte_4 = rand() % 0xFF;

				result += random_byte_1;
				result += random_byte_2;
				result += random_byte_3;
				result += random_byte_4;

				//add esp, 0xblub
				char neger[] = "\x81\xC4\x00\x00\x00\x00";
				*(uint32_t*)&neger[2] = arg_size;
			};

	byte prologue[] = {"\x55\x89\xE5"};
	byte epilogue[] = {"\x5D"};

	byte null_eax[] = {"\x31\xC0"};
	byte mov_eax_randombyte[] = {"\xB8\x00\x00\x00\x00"};
}

static void __stdcall erase(uintptr_t start, uintptr_t end)
{
	uintptr_t size = end - start;

	DWORD protection_flag{};
	VirtualProtect((void*)start, size, PAGE_EXECUTE_READWRITE, &protection_flag);

	for (unsigned int i = 0; i < size; i++)
	{
		int random = rand() % 0x90;
		memset((void*)(start + i), random, 1);
	}

	VirtualProtect((void*)start, size, protection_flag, &protection_flag);
}
