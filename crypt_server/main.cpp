#include "context.h"

c_context ctx;

int main()
{
	ctx.m_server = std::make_unique<c_server>(8123);
	ctx.m_server->run();

    return 0;
}