#include "poqs-common.h"

namespace poqs {
namespace {
#include "poqs-sig_arginfo.h"

struct poqs_sig_t {
    static const char* getTypeName()          { return "SIG"; }
    static int alg_count()                    { return OQS_SIG_alg_count(); }
    static const char* alg_ident(int i)       { return OQS_SIG_alg_identifier(i); }
    static bool alg_enabled(const char* name) { return OQS_SIG_alg_is_enabled(name); }

    using Ptr = detail::UniqueOQS<OQS_SIG, OQS_SIG_new, OQS_SIG_free>;

    Ptr oqs;
    Zeval::String public_key;
    Zeval::String secret_key;
    zend_object std;
};

using SIGObject = detail::Common<poqs_sig_t, OQS_SIG_keypair>;
template<> zend_object_handlers SIGObject::object_handlers;

PHP_METHOD(POQS_SIG, __construct)     { SIGObject::PHP_FN(__construct)(INTERNAL_FUNCTION_PARAM_PASSTHRU); }
PHP_METHOD(POQS_SIG, getMethodName)   { SIGObject::PHP_FN(getMethodName)(INTERNAL_FUNCTION_PARAM_PASSTHRU); }
PHP_METHOD(POQS_SIG, getPublicKey)    { SIGObject::PHP_FN(getPublicKey)(INTERNAL_FUNCTION_PARAM_PASSTHRU); }
PHP_METHOD(POQS_SIG, getSecretKey)    { SIGObject::PHP_FN(getSecretKey)(INTERNAL_FUNCTION_PARAM_PASSTHRU); }
PHP_METHOD(POQS_SIG, algos)           { SIGObject::PHP_FN(algos)(INTERNAL_FUNCTION_PARAM_PASSTHRU); }

#define POQS_STRnLEN(str) reinterpret_cast<uint8_t*>(ZSTR_VAL(str)), ZSTR_LEN(str)

PHP_METHOD(POQS_SIG, sign) {
    zend_string *message, *context = nullptr;

    if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "S|S!", &message, &context) == FAILURE) {
        RETURN_THROWS();
    }

    if (context && !ZSTR_LEN(context)) {
        context = nullptr;
    }

    auto *objval = SIGObject::fromZval(getThis());
    if (!objval->secret_key) {
        zend_throw_exception_ex(poqs_error_exception_ce, 0,
            "Unable to perform sign operation without secret key component");
        RETURN_THROWS();
    }

    Zeval::String signature(objval->oqs->length_signature);
    OQS_STATUS status;
    if (context) {
        if (!OQS_SIG_supports_ctx_str(objval->methodName())) {
            zend_throw_exception_ex(poqs_value_error_ce, 0,
                "OQS signature method '%s' does not support context strings", objval->methodName());
            RETURN_THROWS();
        }

        status = OQS_SIG_sign_with_ctx_str(objval->oqs,
            signature, signature.lenPtr(),
            POQS_STRnLEN(message), POQS_STRnLEN(context), objval->secretKey());
    } else {
        status = OQS_SIG_sign(objval->oqs,
            signature, signature.lenPtr(),
            POQS_STRnLEN(message), objval->secretKey());
    }

    if (status != OQS_SUCCESS) {
        zend_throw_exception_ex(poqs_error_exception_ce, 0, "Failed performing SIG sign operation");
        RETURN_THROWS();
    }

    RETURN_STR(signature.terminate().release());
}

PHP_METHOD(POQS_SIG, verify) {
    zend_string *message, *signature, *context = nullptr;

    if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "SS|S!", &message, &signature, &context) == FAILURE) {
        RETURN_THROWS();
    }

    if (context && !ZSTR_LEN(context)) {
        context = nullptr;
    }

    auto *objval = SIGObject::fromZval(getThis());
    if (!objval->public_key) {
        zend_throw_exception_ex(poqs_error_exception_ce, 0,
            "Unable to perform verify operation without public key component");
        RETURN_THROWS();
    }

    if (context) {
        if (!OQS_SIG_supports_ctx_str(objval->methodName())) {
            zend_throw_exception_ex(poqs_value_error_ce, 0,
                "OQS verify method '%s' does not support context strings", objval->methodName());
            RETURN_THROWS();
        }

        RETURN_BOOL(OQS_SUCCESS == OQS_SIG_verify_with_ctx_str(objval->oqs,
            POQS_STRnLEN(message), POQS_STRnLEN(signature), POQS_STRnLEN(context), objval->publicKey()));
    } else {
        RETURN_BOOL(OQS_SUCCESS == OQS_SIG_verify(objval->oqs,
            POQS_STRnLEN(message), POQS_STRnLEN(signature), objval->publicKey()));
    }
}

PHP_METHOD(POQS_SIG, getParameters) {
    zend_string *method_name;

    if (zend_parse_parameters_throw(ZEND_NUM_ARGS(), "P", &method_name) == FAILURE) {
        RETURN_THROWS();
    }

    SIGObject::impl::Ptr sig(method_name);
    if (!sig) {
        zend_throw_exception_ex(poqs_value_error_ce, 0, "Unknown OQS\\SIG method name '%s'", ZSTR_VAL(method_name));
        RETURN_THROWS();
    }

    array_init(return_value);
    add_assoc_string(return_value, "method_name", sig->method_name);
    add_assoc_string(return_value, "alg_version", sig->alg_version);
#define POQS_PARAM_LONG(name) add_assoc_long(return_value, #name, sig->name)
#define POQS_PARAM_BOOL(name) add_assoc_long(return_value, #name, sig->name)
    POQS_PARAM_LONG(claimed_nist_level);
    POQS_PARAM_BOOL(euf_cma);
    POQS_PARAM_BOOL(suf_cma);
    POQS_PARAM_BOOL(sig_with_ctx_support);
    POQS_PARAM_LONG(length_public_key);
    POQS_PARAM_LONG(length_secret_key);
    POQS_PARAM_LONG(length_signature);
#undef POQS_PARAM_BOOL
#undef POQS_PARAM_LONG
}
}  // namespace

zend_class_entry *poqs_sig_ce = nullptr;
PHP_MINIT_FUNCTION(poqs_sig) {
    poqs_sig_ce = SIGObject::initializeClass(register_class_POQS_SIG());
    return SUCCESS;
}

}  // namespace poqs
