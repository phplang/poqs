#include "poqs-common.h"

namespace poqs {
namespace {
#include "poqs-kem_arginfo.h"

struct poqs_kem_t {
    static const char* getTypeName()          { return "KEM"; }
    static int alg_count()                    { return OQS_KEM_alg_count(); }
    static const char* alg_ident(int i)       { return OQS_KEM_alg_identifier(i); }
    static bool alg_enabled(const char* name) { return OQS_KEM_alg_is_enabled(name); }

    using Ptr = detail::UniqueOQS<OQS_KEM, OQS_KEM_new,OQS_KEM_free>;

    Ptr oqs;
    Zeval::String public_key;
    Zeval::String secret_key;
    zend_object std;
};

using KEMObject = detail::Common<poqs_kem_t, OQS_KEM_keypair>;
template <> zend_object_handlers KEMObject::object_handlers;

PHP_METHOD(POQS_KEM, __construct)     { KEMObject::PHP_FN(__construct)(INTERNAL_FUNCTION_PARAM_PASSTHRU); }
PHP_METHOD(POQS_KEM, getMethodName)   { KEMObject::PHP_FN(getMethodName)(INTERNAL_FUNCTION_PARAM_PASSTHRU); }
PHP_METHOD(POQS_KEM, getPublicKey)    { KEMObject::PHP_FN(getPublicKey)(INTERNAL_FUNCTION_PARAM_PASSTHRU); }
PHP_METHOD(POQS_KEM, getSecretKey)    { KEMObject::PHP_FN(getSecretKey)(INTERNAL_FUNCTION_PARAM_PASSTHRU); }
PHP_METHOD(POQS_KEM, algos)           { KEMObject::PHP_FN(algos)(INTERNAL_FUNCTION_PARAM_PASSTHRU); }

PHP_METHOD(POQS_KEM, newFromSeed) {
    zend_string *method_name, *seed;

    if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "PS", &method_name, &seed) == FAILURE) {
        RETURN_THROWS();
    }

    KEMObject::impl::Ptr kem(method_name);
    if (!kem) {
        zend_throw_exception_ex(poqs_value_error_ce, 0, "Unknown OQS\\KEM method name '%s'", ZSTR_VAL(method_name));
        RETURN_THROWS();
    }

    if (kem->length_keypair_seed == 0) {
        zend_throw_exception_ex(poqs_value_error_ce, 0,
            "Method '%s' does not support keypair generation from seed",
            kem->method_name);
        RETURN_THROWS();
    }

    if (ZSTR_LEN(seed) != kem->length_keypair_seed) {
        const zend_long len = kem->length_keypair_seed;
        POQS_CHECK_LEN_EX(seed, len);
    }

    /* Generate a new key from seed. */
    object_init_ex(return_value, poqs_kem_ce);
    auto *objval = KEMObject::fromZval(return_value);
    objval->oqs = std::move(kem);
    objval->publicKey() = Zeval::String(objval->oqs->length_public_key);
    objval->secretKey() = Zeval::String(objval->oqs->length_secret_key);
    if (OQS_SUCCESS != OQS_KEM_keypair_derand(objval->oqs, objval->publicKey(), objval->secretKey(),
            reinterpret_cast<uint8_t*>(ZSTR_VAL(seed)))) {
        zval_ptr_dtor(return_value);
        RETVAL_NULL();
        zend_throw_exception_ex(poqs_error_exception_ce, 0,
            "Failed generating new keypair for '%s' from seed", objval->methodName());
        RETURN_THROWS();
    }
}

PHP_METHOD(POQS_KEM, encaps) {
    zend_string *seed = nullptr;

    if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "|S!", &seed) == FAILURE) {
        RETURN_THROWS();
    }

    if (seed && !ZSTR_LEN(seed)) {
        seed = nullptr;
    }

    auto *objval = KEMObject::fromZval(getThis());
    if (!objval->publicKey()) {
        zend_throw_exception_ex(poqs_error_exception_ce, 0,
            "Unable to perform encaps operation without public key component");
        RETURN_THROWS();
    }

    Zeval::String shared_secret(objval->oqs->length_shared_secret);
    Zeval::String ciphertext(objval->oqs->length_ciphertext);
    OQS_STATUS status;
    if (seed) {
        if (objval->oqs->length_encaps_seed == 0) {
            zend_throw_exception_ex(poqs_value_error_ce, 0,
                "Method '%s' does not support encaps with seed",
                objval->oqs->method_name);
            RETURN_THROWS();
        }
        POQS_CHECK_LEN_EX(seed, objval->oqs->length_encaps_seed);
        status = OQS_KEM_encaps_derand(objval->oqs,
            ciphertext,
            shared_secret,
            objval->publicKey(),
            reinterpret_cast<uint8_t*>(ZSTR_VAL(seed)));
    } else {
        status = OQS_KEM_encaps(objval->oqs,
            ciphertext,
            shared_secret,
            objval->publicKey());
    }

    if (status != OQS_SUCCESS) {
        zend_throw_exception_ex(poqs_error_exception_ce, 0, "Failed performing KEM encaps operation");
        RETURN_THROWS();
    }

    array_init(return_value);
    add_assoc_str(return_value, "ciphertext", ciphertext.terminate().release());
    add_assoc_str(return_value, "shared_secret", shared_secret.terminate().release());
}

PHP_METHOD(POQS_KEM, decaps) {
    zend_string *ciphertext;

    if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "S", &ciphertext) == FAILURE) {
        RETURN_THROWS();
    }

    auto *objval = KEMObject::fromZval(getThis());
    if (!objval->secret_key) {
        zend_throw_exception_ex(poqs_error_exception_ce, 0,
            "Unable to perform decaps operation without secret key component");
        RETURN_THROWS();
    }

    POQS_CHECK_LEN(ciphertext);
    Zeval::String shared_secret(objval->oqs->length_shared_secret);
    if (OQS_SUCCESS != OQS_KEM_decaps(objval->oqs,
            shared_secret,
            reinterpret_cast<uint8_t*>(ZSTR_VAL(ciphertext)),
            objval->secretKey())) {
        zend_throw_exception_ex(poqs_error_exception_ce, 0, "Failed performing KEM decaps operation");
        RETURN_THROWS();
    }

    RETURN_STR(shared_secret.terminate().release());
}

PHP_METHOD(POQS_KEM, getParameters) {
    zend_string *method_name;

    if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "P", &method_name) == FAILURE) {
        RETURN_THROWS();
    }

    KEMObject::impl::Ptr kem(method_name);
    if (!kem) {
        zend_throw_exception_ex(poqs_value_error_ce, 0, "Unknown OQS\\KEM method name '%s'", ZSTR_VAL(method_name));
        RETURN_THROWS();
    }

    array_init(return_value);
    add_assoc_string(return_value, "method_name", kem->method_name);
    add_assoc_string(return_value, "alg_version", kem->alg_version);
#define POQS_PARAM_LONG(name) add_assoc_long(return_value, #name, kem->name)
    POQS_PARAM_LONG(claimed_nist_level);
    add_assoc_bool(return_value, "ind_cca", kem->ind_cca);
    POQS_PARAM_LONG(length_public_key);
    POQS_PARAM_LONG(length_secret_key);
    POQS_PARAM_LONG(length_ciphertext);
    POQS_PARAM_LONG(length_shared_secret);
    POQS_PARAM_LONG(length_keypair_seed);
    POQS_PARAM_LONG(length_encaps_seed);
#undef POQS_PARAM_LONG
}
}  // namespace

zend_class_entry *poqs_kem_ce = nullptr;
PHP_MINIT_FUNCTION(poqs_kem) {
    poqs_kem_ce = KEMObject::initializeClass(register_class_POQS_KEM());
    return SUCCESS;
}

}  // namespace poqs
