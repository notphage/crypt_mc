#include "context.h"

c_context ctx;

int main()
{
	c_server server(8123);

	server.run();
    return 0;
}