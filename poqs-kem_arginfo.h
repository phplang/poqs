/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: b833690d18443a67e00bf4231cd862dc0f694fb7 */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_POQS_KEM___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, method_name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, public_key, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, secret_key, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_class_POQS_KEM_newFromSeed, 0, 2, POQS\\KEM, 0)
	ZEND_ARG_TYPE_INFO(0, method_name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, seed, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_POQS_KEM_getMethodName, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_POQS_KEM_getPublicKey, 0, 0, IS_STRING, 1)
ZEND_END_ARG_INFO()

#define arginfo_class_POQS_KEM_getSecretKey arginfo_class_POQS_KEM_getPublicKey

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_POQS_KEM_encaps, 0, 0, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, seed, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_POQS_KEM_decaps, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, ciphertext, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_POQS_KEM_algos, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_POQS_KEM_getParameters, 0, 1, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, method_name, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_METHOD(POQS_KEM, __construct);
ZEND_METHOD(POQS_KEM, newFromSeed);
ZEND_METHOD(POQS_KEM, getMethodName);
ZEND_METHOD(POQS_KEM, getPublicKey);
ZEND_METHOD(POQS_KEM, getSecretKey);
ZEND_METHOD(POQS_KEM, encaps);
ZEND_METHOD(POQS_KEM, decaps);
ZEND_METHOD(POQS_KEM, algos);
ZEND_METHOD(POQS_KEM, getParameters);

static const zend_function_entry class_POQS_KEM_methods[] = {
	ZEND_ME(POQS_KEM, __construct, arginfo_class_POQS_KEM___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(POQS_KEM, newFromSeed, arginfo_class_POQS_KEM_newFromSeed, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(POQS_KEM, getMethodName, arginfo_class_POQS_KEM_getMethodName, ZEND_ACC_PUBLIC)
	ZEND_ME(POQS_KEM, getPublicKey, arginfo_class_POQS_KEM_getPublicKey, ZEND_ACC_PUBLIC)
	ZEND_ME(POQS_KEM, getSecretKey, arginfo_class_POQS_KEM_getSecretKey, ZEND_ACC_PUBLIC)
	ZEND_ME(POQS_KEM, encaps, arginfo_class_POQS_KEM_encaps, ZEND_ACC_PUBLIC)
	ZEND_ME(POQS_KEM, decaps, arginfo_class_POQS_KEM_decaps, ZEND_ACC_PUBLIC)
	ZEND_ME(POQS_KEM, algos, arginfo_class_POQS_KEM_algos, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(POQS_KEM, getParameters, arginfo_class_POQS_KEM_getParameters, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_POQS_KEM(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "POQS", "KEM", class_POQS_KEM_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, 0);

	return class_entry;
}
