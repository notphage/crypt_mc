#include "context.h"

c_context ctx;

int main()
{
	ctx.m_server = std::make_unique<c_server>(8123);
	ctx.m_server->run();

	FIPS_mode_set(0);
	CRYPTO_set_locking_callback(NULL);
	CRYPTO_set_id_callback(NULL);
	ENGINE_cleanup();
	CONF_modules_unload(1);
	ERR_free_strings();
	EVP_cleanup();
	CRYPTO_cleanup_all_ex_data();

    return 0;
}