#include "poqs-common.h"


namespace poqs {
zend_class_entry *poqs_rng_ce = nullptr;

namespace {
#include "poqs-rng_arginfo.h"

void poqs_random_bytes(uint8_t *ptr, size_t sz) {
    if (!POQSG(prng_data)) {
        zend_throw_exception_ex(poqs_error_exception_ce, 0, "Failed reading psuedo-random data, no data available");
        return;
    }

    if (sz > (ZSTR_LEN(POQSG(prng_data)) - POQSG(prng_pos))) {
        zend_throw_exception_ex(poqs_error_exception_ce, 0, "Insufficient psuedo-random data available");
        return;
    }

    memcpy(ptr, ZSTR_VAL(POQSG(prng_data)) + POQSG(prng_pos), sz);
    POQSG(prng_pos) += sz;
}

PHP_METHOD(POQS_PRNG, __construct) {
    /* Uncallable */
    zend_throw_exception_ex(poqs_error_exception_ce, 0, "Called private constructor");
    RETURN_THROWS();
}

PHP_METHOD(POQS_PRNG, getRemainingLength) {
    if (zend_parse_parameters_none_throw() == FAILURE) {
        RETURN_THROWS();
    }

    if (!POQSG(prng_data)) { RETURN_LONG(0); }
    RETURN_LONG(ZSTR_LEN(POQSG(prng_data)) - POQSG(prng_pos));
}

PHP_METHOD(POQS_PRNG, getRemainingData) {
    if (zend_parse_parameters_none_throw() == FAILURE) {
        RETURN_THROWS();
    }

    if (!POQSG(prng_data)) { RETURN_EMPTY_STRING(); }
    if (!POQSG(prng_pos)) {
        zend_string_addref(POQSG(prng_data));
        RETURN_STR(POQSG(prng_data));
    }
    RETURN_STRINGL(ZSTR_VAL(POQSG(prng_data)) + POQSG(prng_pos), ZSTR_LEN(POQSG(prng_data)) - POQSG(prng_pos));
}

PHP_METHOD(POQS_PRNG, setData) {
    zend_string *prng_data;

    if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "S", &prng_data) == FAILURE) {
        RETURN_THROWS();
    }

    if (POQSG(prng_data)) {
        zend_string_release(POQSG(prng_data));
    }

    zend_string_addref(prng_data);
    POQSG(prng_data) = prng_data;
    POQSG(prng_pos) = 0;
    OQS_randombytes_custom_algorithm(poqs_random_bytes);
}

PHP_METHOD(POQS_PRNG, useSystem) {
    if (zend_parse_parameters_none_throw() == FAILURE) {
        RETURN_THROWS();
    }

    OQS_randombytes_switch_algorithm(OQS_RAND_alg_system);
    if (POQSG(prng_data)) {
        zend_string_release(POQSG(prng_data));
        POQSG(prng_data) = nullptr;
        POQSG(prng_pos) = 0;
    }
}

PHP_METHOD(POQS_PRNG, useOpenSSL) {
    if (zend_parse_parameters_none_throw() == FAILURE) {
        RETURN_THROWS();
    }

    OQS_randombytes_switch_algorithm(OQS_RAND_alg_openssl);
    if (POQSG(prng_data)) {
        zend_string_release(POQSG(prng_data));
        POQSG(prng_data) = nullptr;
        POQSG(prng_pos) = 0;
    }
}
}  // namespace

PHP_MINIT_FUNCTION(poqs_rng) {
    poqs_rng_ce = register_class_POQS_PRNG();
    return SUCCESS;
}
}  // namespace poqs
