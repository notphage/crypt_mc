#include "context.h"

c_context ctx;

int main()
{
	try
	{
		ctx.m_server = std::make_unique<c_server>(8123);
		ctx.m_server->run();
	}
	catch (std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}

    return 0;
}