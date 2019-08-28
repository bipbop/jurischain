#include <stdio.h>
#include <time.h>
#include <jurischain.h>

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "ext/standard/info.h"
#include "php_jurischain.h"
#include "zend_exceptions.h"

int res_num = 0;
char *LE_JURISCHAIN = "jurischain_res\0";

static void jurischain_res_dtor(zend_resource *rsrc)
{
    jurischain_destroy((jurischain_ctx_t **)&rsrc->ptr);
}

PHP_RINIT_FUNCTION(jurischain)
{
	return SUCCESS;
}

PHP_MINIT_FUNCTION(jurischain)
{
	res_num = zend_register_list_destructors_ex(jurischain_res_dtor, NULL, LE_JURISCHAIN, module_number);
	return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(jurischain)
{
	return SUCCESS;
}

PHP_FUNCTION(jurischain_gen)
{
	char *seed;
	long difficult;
	size_t seed_length;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_LONG(difficult)
		Z_PARAM_STRING(seed, seed_length)
  ZEND_PARSE_PARAMETERS_END();

	jurischain_ctx_t *pow = jurischain_init();
	if (!pow) {
		RETURN_NULL()
		return;
	}

	jurischain_gen(pow, difficult, seed, seed_length);

	RETURN_RES(zend_register_resource(pow, res_num));
}

PHP_FUNCTION(jurischain_try)
{
	zval *challenge = NULL;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_RESOURCE(challenge)
  ZEND_PARSE_PARAMETERS_END();

	RETURN_BOOL(jurischain_try((jurischain_ctx_t *)zend_fetch_resource_ex(challenge, LE_JURISCHAIN, res_num)));
}

PHP_FUNCTION(jurischain_verify)
{
	zval *challenge = NULL;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_RESOURCE(challenge)
  ZEND_PARSE_PARAMETERS_END();

	RETURN_BOOL(jurischain_verify((jurischain_ctx_t *)zend_fetch_resource_ex(challenge, LE_JURISCHAIN, res_num)));
}

PHP_FUNCTION(jurischain_get)
{
	zval *challenge = NULL;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_RESOURCE(challenge)
  ZEND_PARSE_PARAMETERS_END();

	jurischain_ctx_t * ctx = (jurischain_ctx_t *)zend_fetch_resource_ex(challenge, LE_JURISCHAIN, res_num);
	char str[(HASH_LEN * 2) + 1] = { 0, };
  for (int i = 0; i < HASH_LEN; i++) sprintf(str + (i * 2), "%02hhX", ctx->seed[i]);
	RETURN_STRING(str);
}

PHP_FUNCTION(jurischain_set)
{
	zval *challenge = NULL;
	char *seed = NULL;
	size_t seed_len = 0;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_RESOURCE(challenge)
		Z_PARAM_STRING(seed, seed_len)
  ZEND_PARSE_PARAMETERS_END();

	ZEND_ASSERT(Z_TYPE_P(challenge) == IS_RESOURCE); /* just a check to be sure */

	if (seed_len < HASH_LEN*2) {
		RETURN_BOOL(0);
		return;
	}
	jurischain_ctx_t *ctx = (jurischain_ctx_t *)zend_fetch_resource_ex(challenge, LE_JURISCHAIN, res_num);
  for (int i = 0; i < HASH_LEN; i++) {
		if (sscanf(seed + (i * 2), "%02hhX", &ctx->seed[i]) != 1) {
			RETURN_BOOL(0);
			return;
		}
	}
	RETURN_BOOL(1);
}

PHP_MINFO_FUNCTION(jurischain)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "jurischain support", "enabled");
	php_info_print_table_end();
}

ZEND_BEGIN_ARG_INFO(arginfo_jurischain_gen, 0)
    ZEND_ARG_INFO(0, _complexity)
    ZEND_ARG_INFO(0, _seed)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_jurischain_default, 0)
    ZEND_ARG_INFO(0, _jurischain_res)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_jurischain_set, 0)
    ZEND_ARG_INFO(0, _jurischain_res)
    ZEND_ARG_INFO(0, _jurischain_str)
ZEND_END_ARG_INFO()


const zend_function_entry jurischain_functions[] = {
	PHP_FE(jurischain_gen, arginfo_jurischain_gen)
	PHP_FE(jurischain_set, arginfo_jurischain_set)
	PHP_FE(jurischain_try, arginfo_jurischain_default)
	PHP_FE(jurischain_get, arginfo_jurischain_default)
	PHP_FE(jurischain_verify, arginfo_jurischain_default)
	PHP_FE_END
};

zend_module_entry jurischain_module_entry = {
	STANDARD_MODULE_HEADER,
	"jurischain",
	jurischain_functions,
	PHP_MINIT(jurischain),
	PHP_MSHUTDOWN(jurischain),
	PHP_RINIT(jurischain),
	NULL,
	PHP_MINFO(jurischain),
	PHP_JURISCHAIN_VERSION,
	STANDARD_MODULE_PROPERTIES
};


#ifdef COMPILE_DL_JURISCHAIN
# ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
# endif
ZEND_GET_MODULE(jurischain)
#endif
