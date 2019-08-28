#include <stdio.h>
#include <time.h>
#include <juschain.h>

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "ext/standard/info.h"
#include "php_juschain.h"
#include "zend_exceptions.h"

int res_num;
char *LE_JUSCHAIN = "juschain_res\0";

static void juschain_res_dtor(zend_resource *rsrc)
{
    pow_challenge_destroy((pow_ctx_t **)&rsrc->ptr);
}

PHP_RINIT_FUNCTION(juschain)
{
	return SUCCESS;
}

PHP_MINIT_FUNCTION(juschain)
{
	res_num = zend_register_list_destructors_ex(juschain_res_dtor, NULL, LE_JUSCHAIN, module_number);
	return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(juschain)
{
	return SUCCESS;
}

PHP_FUNCTION(juschain_gen)
{
	char *seed;
	long difficult;
	size_t seed_length;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_LONG(difficult)
		Z_PARAM_STRING(seed, seed_length)
  ZEND_PARSE_PARAMETERS_END();

	pow_ctx_t *pow = pow_challenge_init();
	if (!pow) {
		RETURN_NULL()
		return;
	}

	pow_gen(pow, difficult, seed, seed_length);

	RETURN_RES(zend_register_resource(pow, res_num));
}

PHP_FUNCTION(juschain_try)
{
	zval *challenge;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_RESOURCE(challenge)
  ZEND_PARSE_PARAMETERS_END();

	RETURN_BOOL(pow_try((pow_ctx_t *)zend_fetch_resource_ex(challenge, LE_JUSCHAIN, res_num)));
}

PHP_FUNCTION(juschain_verify)
{
	zval *challenge;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_RESOURCE(challenge)
  ZEND_PARSE_PARAMETERS_END();

	RETURN_BOOL(pow_verify((pow_ctx_t *)zend_fetch_resource_ex(challenge, LE_JUSCHAIN, res_num)));
}

PHP_FUNCTION(juschain_get)
{
	zval *challenge;

	ZEND_PARSE_PARAMETERS_START(1, 1)
		Z_PARAM_RESOURCE(challenge)
  ZEND_PARSE_PARAMETERS_END();

	pow_ctx_t * ctx = (pow_ctx_t *)zend_fetch_resource_ex(challenge, LE_JUSCHAIN, res_num);
	char str[(HASH_LEN * 2) + 1] = { 0, };
  for (int i = 0; i < HASH_LEN; i++) sprintf(str + (i * 2), "%02hhX", ctx->seed[i]);
	RETURN_STRING(str);
}

PHP_FUNCTION(juschain_set)
{
	zval *challenge;
	char *seed;
	size_t seed_len;

	ZEND_PARSE_PARAMETERS_START(2, 2)
		Z_PARAM_RESOURCE(challenge)
		Z_PARAM_STRING(seed, seed_len)
  ZEND_PARSE_PARAMETERS_END();

	ZEND_ASSERT(Z_TYPE_P(challenge) == IS_RESOURCE); /* just a check to be sure */

	if (seed_len < HASH_LEN*2) {
		RETURN_BOOL(0);
		return;
	}
	pow_ctx_t *ctx = (pow_ctx_t *)zend_fetch_resource_ex(challenge, LE_JUSCHAIN, res_num);
  for (int i = 0; i < HASH_LEN; i++) {
		if (sscanf(seed + (i * 2), "%02hhX", &ctx->seed[i]) != 1) {
			RETURN_BOOL(0);
			return;
		}
	}
	RETURN_BOOL(1);
}

PHP_MINFO_FUNCTION(juschain)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "juschain support", "enabled");
	php_info_print_table_end();
}

ZEND_BEGIN_ARG_INFO(arginfo_juschain_gen, 0)
    ZEND_ARG_INFO(0, _complexity)
    ZEND_ARG_INFO(0, _seed)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_juschain_default, 0)
    ZEND_ARG_INFO(0, _juschain_res)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arginfo_juschain_set, 0)
    ZEND_ARG_INFO(0, _juschain_res)
    ZEND_ARG_INFO(0, _juschain_str)
ZEND_END_ARG_INFO()


const zend_function_entry juschain_functions[] = {
	PHP_FE(juschain_gen, arginfo_juschain_gen)
	PHP_FE(juschain_set, arginfo_juschain_set)
	PHP_FE(juschain_try, arginfo_juschain_default)
	PHP_FE(juschain_get, arginfo_juschain_default)
	PHP_FE(juschain_verify, arginfo_juschain_default)
	PHP_FE_END
};

zend_module_entry juschain_module_entry = {
	STANDARD_MODULE_HEADER,
	"juschain",
	juschain_functions,
	PHP_MINIT(juschain),
	PHP_MSHUTDOWN(juschain),
	PHP_RINIT(juschain),
	NULL,
	PHP_MINFO(juschain),
	PHP_JUSCHAIN_VERSION,
	STANDARD_MODULE_PROPERTIES
};


#ifdef COMPILE_DL_JUSCHAIN
# ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
# endif
ZEND_GET_MODULE(juschain)
#endif
