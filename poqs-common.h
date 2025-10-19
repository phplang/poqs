#pragma once

#include "zeval-string.h"
#include "poqs.h"
#include "Zend/zend_exceptions.h"
#include "Zend/zend_smart_str.h"

#include <new>
#include <oqs/oqs.h>

namespace poqs {

/* poqs.cpp */
extern zend_class_entry *poqs_error_exception_ce;
extern zend_class_entry *poqs_value_error_ce;

/* poqs-kem.cpp */
extern zend_class_entry *poqs_kem_ce;
PHP_MINIT_FUNCTION(poqs_kem);
void poqs_kem_minfo();

/* poqs-rng.cpp */
PHP_MINIT_FUNCTION(poqs_rng);

/* poqs-sig.cpp */
extern zend_class_entry *poqs_sig_ce;
PHP_MINIT_FUNCTION(poqs_sig);
void poqs_sig_minfo();

namespace detail {

template <typename OQST>
using OQSNew = OQST*(const char*);

template <typename OQST>
using OQSKeyPair = OQS_STATUS(const OQST*, uint8_t*, uint8_t*);

template <typename OQST>
using OQSDeleter = void(OQST*);

/**
 * std::unique_ptr analogue which is explicitly OQS method aware.
 */
template <typename OQST, OQSNew<OQST>* ctor, OQSDeleter<OQST>* dtor>
class UniqueOQS {
public:
    using impl = OQST;

    UniqueOQS() : _ptr(nullptr) {}
    explicit UniqueOQS(const char *method_name) : _ptr(ctor(method_name)) {}
    explicit UniqueOQS(const zend_string *method_name) : _ptr(ctor(ZSTR_VAL(method_name))) {}
    explicit UniqueOQS(OQST* ptr): _ptr(ptr) {}
    UniqueOQS(UniqueOQS&& src) {
        _ptr = src._ptr;
        src._ptr = nullptr;
    }
    UniqueOQS& operator=(UniqueOQS&& src) {
        if (_ptr) {
            dtor(_ptr);
        }
        _ptr = src._ptr;
        src._ptr = nullptr;
        return *this;
    }
    UniqueOQS(const UniqueOQS& src) {
        if (auto* sptr = src._ptr) {
            _ptr = ctor(sptr->method_name);
        } else {
            _ptr = nullptr;
        }
    }
    UniqueOQS& operator=(const UniqueOQS& src) {
        if (_ptr) {
            dtor(_ptr);
        }
        if (auto* sptr = src._ptr) {
            _ptr = ctor(sptr->method_name);
        } else {
            _ptr = nullptr;
        }
    }
    ~UniqueOQS() {
        if (_ptr) {
            dtor(_ptr);
            _ptr = nullptr;
        }
    }

    UniqueOQS& reset(const char* method_name) {
        if (_ptr) {
            dtor(_ptr);
        }
        _ptr = ctor(method_name);
        return *this;
    }

    UniqueOQS& reset(const zend_string* method_name) {
        return reset(reinterpret_cast<const char*>(ZSTR_VAL(method_name)));
    }

    static UniqueOQS& create(const char* method_name) {
        return UniqueOQS(ctor(method_name));
    }

    operator const OQST*() const { return _ptr; }
    operator OQST*() { return _ptr; }

    const OQST* get() const { return _ptr; }
    OQST* get() { return _ptr; }

    const OQST* operator->() const { return _ptr; }
    OQST* operator->() { return _ptr; }

    operator bool() const {
        return _ptr != nullptr;
    }
private:
    OQST *_ptr;
};

#define POQS_CHECK_LEN_EX(param, expect_len) \
    if (ZSTR_LEN(param) != expect_len) { \
        zend_throw_exception_ex(poqs_value_error_ce, 0, \
            "Invalid length for " #param " expected " ZEND_LONG_FMT " got " ZEND_LONG_FMT, \
            expect_len, ZSTR_LEN(param)); \
        RETURN_THROWS(); \
    }

#define POQS_CHECK_LEN(param) POQS_CHECK_LEN_EX(param, objval->oqs->length_##param)

/**
 * Internal storage for a zend_object representing an OQS instance.
 * Contains an OQS method implementation {oqs}, and keypair {public_key}/{private_key}.
 */
template <typename ZObj, OQSKeyPair<typename ZObj::Ptr::impl>* keypair>
class Common : public ZObj {
public:
    using impl = ZObj;

    Common() = default;
    ~Common() = default;
    Common(const Common&) = default;
    Common(Common&&) = default;

    static Common* fromZendObject(zend_object *obj) {
        return reinterpret_cast<Common*>(obj + 1) - 1;
    }
    static Common* fromZval(zval *pzv) { return fromZendObject(Z_OBJ_P(pzv)); }
    static Common* fromZval(zval &pzv) { return fromZendObject(Z_OBJ(pzv)); }

    zend_object* toZendObject() {
        return reinterpret_cast<zend_object*>(this + 1) - 1;
    }

    const char* methodName() const {
        return const_cast<Common*>(this)->oqs->method_name;
    }

    Zeval::String& publicKey() {
        return this->public_key;
    }

    Zeval::String& secretKey() {
        return this->secret_key;
    }

    OQS_STATUS makeKeyPair(uint8_t *public_key, uint8_t *secret_key) const {
        if (!this->oqs) {
            return OQS_ERROR;
        }
        return keypair(this->oqs, public_key, secret_key);
    }

    static zend_object_handlers object_handlers;

    static zend_object* object_ctor(zend_class_entry *ce) {
        auto* zobj = reinterpret_cast<Common*>(
            ecalloc(1, sizeof(Common) + zend_object_properties_size(ce)));
        new (zobj) Common();

        auto* ret = zobj->toZendObject();
        zend_object_std_init(ret, ce);
        object_properties_init(ret, ce);
        ret->handlers = &object_handlers;

        return ret;
    }

    static void object_free(zend_object *zobj) {
        fromZendObject(zobj)->~Common();
        zend_object_std_dtor(zobj);
    }

    static zend_object* object_clone(zend_object *zsrc) {
        auto *zdst = object_ctor(zsrc->ce);
        zend_objects_clone_members(zdst, zsrc);

        auto *src = fromZendObject(zsrc);
        auto *dst = fromZendObject(zdst);

        dst->oqs = typename ZObj::Ptr(src->methodName());
        if (!dst->oqs) {
            object_free(zdst);
            return nullptr;
        }

        dst->publicKey() = src->publicKey();
        dst->secretKey() = src->secretKey();

        return zdst;
    }

    static zend_class_entry* initializeClass(zend_class_entry *ce) {
        ce->create_object = object_ctor;

        memcpy(&object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
        object_handlers.offset = XtOffsetOf(Common, std);
        object_handlers.clone_obj = object_clone;
        object_handlers.free_obj = object_free;

        return ce;
    }

    static PHP_FUNCTION(__construct) {
        zend_string *method_name, *public_key = nullptr, *secret_key = nullptr;

        if (zend_parse_parameters_throw(ZEND_NUM_ARGS(),
            "P|S!S!", &method_name, &public_key, &secret_key) == FAILURE) {
            RETURN_THROWS();
        }

        auto *objval = fromZval(getThis());
        objval->oqs.reset(method_name);
        if (!objval->oqs) {
            zend_throw_exception_ex(
                poqs_value_error_ce, 0, "Unknown OQS\\%s method name '%s'",
                ZObj::getTypeName(), ZSTR_VAL(method_name));
            RETURN_THROWS();
        }

        if (!public_key && !secret_key) {
            /* Generate a new key */
            objval->publicKey() = Zeval::String(objval->oqs->length_public_key);
            objval->secretKey() = Zeval::String(objval->oqs->length_secret_key);
            if (OQS_SUCCESS != objval->makeKeyPair(objval->publicKey(), objval->secretKey())) {
                zend_throw_exception_ex(poqs_error_exception_ce, 0,
                    "Failed generating new keypair for '%s'", objval->methodName());
                RETURN_THROWS();
            }
            return;
        }

        /**
         *  Note that it is technically permitted to provide only secret_key without public_key,
         * as caller may call decaps methods only, though this seems non-sensical in practice.
         */
        if (public_key) {
            POQS_CHECK_LEN(public_key);
            objval->publicKey() = std::move(Zeval::String(public_key));
        }

        if (secret_key) {
            POQS_CHECK_LEN(secret_key);
            objval->secretKey() = std::move(Zeval::String(secret_key));
        }

    }

    static PHP_FUNCTION(getMethodName) {
        if (zend_parse_parameters_none_throw() == FAILURE) {
            RETURN_THROWS();
        }

        RETURN_STRING(fromZval(getThis())->methodName());
    }

    static PHP_FUNCTION(getPublicKey) {
        if (zend_parse_parameters_none_throw() == FAILURE) {
            RETURN_THROWS();
        }

        auto& key = fromZval(getThis())->publicKey();
        if (!key) {
            RETURN_NULL();
        }

        RETURN_STR(key.addref());
    }

    static PHP_FUNCTION(getSecretKey) {
        if (zend_parse_parameters_none_throw() == FAILURE) {
            RETURN_THROWS();
        }

        auto& key = fromZval(getThis())->secretKey();
        if (!key) {
            RETURN_NULL();
        }

        RETURN_STR(key.addref());
    }

    static PHP_FUNCTION(algos) {
        if (zend_parse_parameters_none_throw() == FAILURE) {
            RETURN_THROWS();
        }

        const int count = ZObj::alg_count();
        array_init(return_value);
        for (int i = 0; i < count; ++i) {
            const char *name = ZObj::alg_ident(i);
            if (!name) continue;
            if (!ZObj::alg_enabled(name)) continue;
            add_next_index_string(return_value, name);
        }
    }
};
}  // namespace detail

}  // namespace poqs
