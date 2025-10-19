#pragma once

#include "poqs-common.h"

namespace poqs {

class KemObject {
public:
    KemObject() = delete;
    KemObject& KemObject(const KemObject&) = delete;
    KemObject& operator=(const KemObject&) = delete;

    static KemObject* fromZendObject(zend_object *zobj) {
        return reinterpret_cast<KemObject*>(zobj + 1) - 1;
    }

    zend_object* toZendObject() {
        return reinterpret_cast<zend_object*>(this + 1) - 1;
    }

public:
    OQS_KEM *kem;
    zend_string *public_key;
    zend_string *secert_key;
    zend_object std;
};

extern zend_class_entry *poqs_kem_ce;
PHP_MINIT_FUNCTION(poqs_kem);
void poqs_kem_minfo();

}  // namespace poqs
