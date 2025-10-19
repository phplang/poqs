/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 6e606b40ca4e4eee23aba0449a1e69bdfd9fe510 */

ZEND_BEGIN_ARG_INFO_EX(arginfo_class_POQS_PRNG___construct, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_POQS_PRNG_getRemainingLength, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_POQS_PRNG_getRemainingData, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_POQS_PRNG_setData, 0, 1, IS_VOID, 0)
	ZEND_ARG_TYPE_INFO(0, data, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_class_POQS_PRNG_useSystem, 0, 0, IS_VOID, 0)
ZEND_END_ARG_INFO()

#define arginfo_class_POQS_PRNG_useOpenSSL arginfo_class_POQS_PRNG_useSystem

ZEND_METHOD(POQS_PRNG, __construct);
ZEND_METHOD(POQS_PRNG, getRemainingLength);
ZEND_METHOD(POQS_PRNG, getRemainingData);
ZEND_METHOD(POQS_PRNG, setData);
ZEND_METHOD(POQS_PRNG, useSystem);
ZEND_METHOD(POQS_PRNG, useOpenSSL);

static const zend_function_entry class_POQS_PRNG_methods[] = {
	ZEND_ME(POQS_PRNG, __construct, arginfo_class_POQS_PRNG___construct, ZEND_ACC_PRIVATE)
	ZEND_ME(POQS_PRNG, getRemainingLength, arginfo_class_POQS_PRNG_getRemainingLength, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(POQS_PRNG, getRemainingData, arginfo_class_POQS_PRNG_getRemainingData, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(POQS_PRNG, setData, arginfo_class_POQS_PRNG_setData, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(POQS_PRNG, useSystem, arginfo_class_POQS_PRNG_useSystem, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_ME(POQS_PRNG, useOpenSSL, arginfo_class_POQS_PRNG_useOpenSSL, ZEND_ACC_PUBLIC|ZEND_ACC_STATIC)
	ZEND_FE_END
};

static zend_class_entry *register_class_POQS_PRNG(void)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "POQS", "PRNG", class_POQS_PRNG_methods);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, 0);

	return class_entry;
}
