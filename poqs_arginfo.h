/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 4fa28d0a5d636966fe46497b3b7492080b18c5ad */

static void register_poqs_symbols(int module_number)
{
	REGISTER_STRING_CONSTANT("POQS\\VERSION", POQS_OQS_VERSION, CONST_PERSISTENT);
	REGISTER_STRING_CONSTANT("POQS\\VERSION_COMPILED", OQS_VERSION_TEXT, CONST_PERSISTENT);
}

static zend_class_entry *register_class_POQS_ErrorException(zend_class_entry *class_entry_ErrorException)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "POQS", "ErrorException", NULL);
	class_entry = zend_register_internal_class_with_flags(&ce, class_entry_ErrorException, 0);

	return class_entry;
}

static zend_class_entry *register_class_POQS_ValueError(zend_class_entry *class_entry_ValueError)
{
	zend_class_entry ce, *class_entry;

	INIT_NS_CLASS_ENTRY(ce, "POQS", "ValueError", NULL);
	class_entry = zend_register_internal_class_with_flags(&ce, class_entry_ValueError, 0);

	return class_entry;
}
