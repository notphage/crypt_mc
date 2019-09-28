static unsigned long int next = 1;

extern "C" int __cdecl rand(void)
{
	next = next * 1103515245 + 12345;
	return (unsigned int)(next / 65536) % 32768;
}

extern "C" void __cdecl srand(unsigned int seed)
{
	next = seed;
}

namespace std
{
	unsigned int __cdecl _Random_device(void)
	{
		return rand();
	}
}
