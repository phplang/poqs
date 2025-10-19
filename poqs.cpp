#include "poqs-common.h"
#include "ext/standard/info.h"

/* Used by poqs.stub.php to pick up value for \POQS\VERSION */
#define POQS_OQS_VERSION OQS_version()

namespace poqs {
#include "poqs_arginfo.h"

PHPAPI ZEND_DECLARE_MODULE_GLOBALS(poqs);
zend_class_entry *poqs_error_exception_ce;
zend_class_entry *poqs_value_error_ce;

namespace {
PHP_MINIT_FUNCTION(poqs) {
    OQS_init();

    register_poqs_symbols(module_number);
    poqs_error_exception_ce = register_class_POQS_ErrorException(zend_ce_error_exception);
    poqs_value_error_ce = register_class_POQS_ValueError(zend_ce_value_error);

    return ((1 == 1)
        && (PHP_MINIT(poqs_kem)(INIT_FUNC_ARGS_PASSTHRU) == SUCCESS)
        && (PHP_MINIT(poqs_rng)(INIT_FUNC_ARGS_PASSTHRU) == SUCCESS)
        && (PHP_MINIT(poqs_sig)(INIT_FUNC_ARGS_PASSTHRU) == SUCCESS)
        ) ? SUCCESS : FAILURE;
}

template <typename IdFunc, typename EnabledFunc>
void poqs_minfo_algs(const char* alg_type, const int count, IdFunc idFunc, EnabledFunc enabledFunc) {
    const auto alg_type_len = strlen(alg_type);
    smart_str enabled = {0}, disabled = {0};

    if (!count) {
        return;
    }

    smart_str_alloc(&enabled, (count * 12) + 1, 0);
    smart_str_alloc(&disabled, (count * 12) + 1, 0);
    for (int i = 0; i < count; ++i) {
        const char *name = idFunc(i);
        if (!name) continue;
        smart_str *endis = enabledFunc(name) ? &enabled : &disabled;
        smart_str_appends(endis, name);
        smart_str_appends(endis, ", ");
    }

    if (smart_str_get_len(&enabled) > 2) {
        auto *label = zend_string_alloc(sizeof("Enabled s") + alg_type_len, 0);
        snprintf(ZSTR_VAL(label), ZSTR_LEN(label), "Enabled %ss", alg_type);

        auto *str = smart_str_extract(&enabled);
        /* Remove trailing ", " */
        ZSTR_VAL(str)[ZSTR_LEN(str)-2] = 0;
        php_info_print_table_row(2, ZSTR_VAL(label), ZSTR_VAL(str));
        zend_string_release(str);
        zend_string_release(label);
    } else {
        smart_str_free(&enabled);
    }

    if (smart_str_get_len(&disabled) > 2) {
        auto *label = zend_string_alloc(sizeof("Available, but disabled s") + alg_type_len, 0);
        snprintf(ZSTR_VAL(label), ZSTR_LEN(label), "Available, but disabled %ss", alg_type);

        zend_string *str = smart_str_extract(&disabled);
        /* Remove trailing ", " */
        ZSTR_VAL(str)[ZSTR_LEN(str)-2] = 0;
        php_info_print_table_row(2, ZSTR_VAL(label), ZSTR_VAL(str));
        zend_string_release(str);
        zend_string_release(label);
    } else {
        smart_str_free(&disabled);
    }
}
PHP_MINFO_FUNCTION(poqs) {
    php_info_print_table_start();
    php_info_print_table_row(2, "liboqs (compiled)", OQS_VERSION_TEXT);
    php_info_print_table_row(2, "liboqs (linked)", OQS_version());
    poqs_minfo_algs("KEM", OQS_KEM_alg_count(), OQS_KEM_alg_identifier, OQS_KEM_alg_is_enabled);
    poqs_minfo_algs("SIG", OQS_SIG_alg_count(), OQS_SIG_alg_identifier, OQS_SIG_alg_is_enabled);
    php_info_print_table_end();
}

PHP_GINIT_FUNCTION(poqs) {
#ifdef ZTS
    ZEND_TSRMLS_CACHE_UPDATE();
#endif

    poqs_globals->prng_data = nullptr;
    poqs_globals->prng_pos = 0;
}
}  // namespace

#ifdef COMPILE_DL_POQS
namespace {
#endif
zend_module_entry poqs_module_entry = {
    STANDARD_MODULE_HEADER,
    "poqs",
    nullptr, /* functions */
    PHP_MINIT(poqs),
    nullptr, /* MSHUTDOWN */
    nullptr, /* RINIT */
    nullptr, /* RSHUTDOWN */
    PHP_MINFO(poqs),
    NO_VERSION_YET,
    PHP_MODULE_GLOBALS(poqs),
    PHP_GINIT(poqs),
    nullptr, /* GSHUTDOWN */
    nullptr, /* RPOSTSHUTDOWN */
    STANDARD_MODULE_PROPERTIES_EX
};
#ifdef COMPILE_DL_POQS
}  // namespace

ZEND_GET_MODULE(poqs)
#endif

}  // namespace poqs
