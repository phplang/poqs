/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 3dddeeb04b38de8cea8e43731a4d143fb1b593fa */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_POQS_SIG___construct, 0, 0, 1)
	ZEND_ARG_TYPE_INFO(0, method_name, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, public_key, IS_STRING, 1, "null")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, secret_key, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_POQS_SIG_getMethodName, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_POQS_SIG_getPublicKey, 0, 0, IS_STRING, 1)
ZEND_END_ARG_INFO()

#define arginfo_class_POQS_SIG_getSecretKey arginfo_class_POQS_SIG_getPublicKey

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_POQS_SIG_sign, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, message, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, context, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_POQS_SIG_verify, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, message, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, signature, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, context, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_POQS_SIG_algos, 0, 0, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_POQS_SIG_getParameters, 0, 1, IS_ARRAY, 0)
	ZEND_ARG_TYPE_INFO(0, method_name, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_METHOD(POQS_SIG, __construct);
ZEND_METHOD(POQS_SIG, getMethodName);
ZEND_METHOD(POQS_SIG, getPublicKey);
ZEND_METHOD(POQS_SIG, getSecretKey);
ZEND_METHOD(POQS_SIG, sign);
ZEND_METHOD(POQS_SIG, verify);
ZEND_METHOD(POQS_SIG, algos);
ZEND_METHOD(POQS_SIG, getParameters);

static const zend_function_entry class_POQS_SIG_methods[] = {
	ZEND_ME(POQS_SIG, __construct, arginfo_class_POQS_SIG___construct, ZEND_ACC_PUBLIC)
	ZEND_ME(POQS_SIG, getMethodName, arginfo_class_POQS_SIG_getMethodName, ZEND_ACC_PUBLIC)
	ZEND_ME(POQS_SIG, getPublicKey, arginfo_class_POQS_SIG_getPublicKey, ZEND_ACC_PUBLIC)
	ZEND_ME(POQS_SIG, getSecretKey, arginfo_class_POQS_SIG_getSecretKey, ZEND_ACC_PUBLIC)
	ZEND_ME(POQS_SIG, sign, arginfo_class_POQS_SIG_sign, ZEND_ACC_PUBLIC)
	ZEND_ME(POQS_SIG, verify, arginfo_class_POQS_SIG_verify, ZEND_ACC_PUBLIC)
	ZEND_ME(POQS_SIG, algos, arginfo_class_POQS_SIG_algos, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(POQS_SIG, getParameters, arginfo_class_POQS_SIG_getParameters, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_POQS_SIG(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "POQS", "SIG", class_POQS_SIG_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, 0);

	return class_entry;
}
