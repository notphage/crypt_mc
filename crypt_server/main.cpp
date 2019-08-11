#include "context.h"

c_context ctx;

int main()
{
	try
	{
		c_server server(8123);
		server.run();
	}
	catch (std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}

    return 0;
}