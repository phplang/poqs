#pragma once

#include "php.h"

namespace Zeval {

class String {
public:
    String() : _str(nullptr) {}
    explicit String(std::nullptr_t) : _str(nullptr) {}
    String(const String& str) : String(const_cast<String*>(&str)->get()) {}
    String& operator=(const String& str) {
        if (_str) {
            zend_string_release(_str);
        }
        _str = str._str;
        if (_str) {
            zend_string_addref(_str);
        }
        return *this;
    }

    String(String&& str) = default;
    String& operator=(String&& str) {
        if (_str) {
            zend_string_release(_str);
        }
        _str = str._str;
        str._str = nullptr;
        return *this;
    }

    explicit String(size_t len, bool persistent = false) : _str(zend_string_alloc(len, persistent)) {}
    String(const char *str, size_t len, bool persistent = false) : _str(zend_string_init(str, len, persistent)) {}
    explicit String(const char *str, bool persistent = false) : String(str, strlen(str), persistent) {}

    explicit String(zend_string *str) : _str(str) {
        if (_str) {
            zend_string_addref(_str);
        }
    }

    ~String() {
        if (_str) {
            zend_string_release(_str);
        }
    }

    bool empty() const {
        return (_str == nullptr) || (ZSTR_LEN(_str) == 0);
    }

    operator bool() const {
        return !empty();
    }

    bool isWritable() const {
        return _str ? (GC_REFCOUNT(_str) == 1) : false;
    }

    // Const inspectors can come from zend_empty_string to present a empty substitue for nullptr.
    template <typename CharT = char>
    const CharT* val() const { return reinterpret_cast<CharT*>(ZSTR_VAL(getOrEmpty())); }
    const size_t len() const { return ZSTR_LEN(getOrEmpty()); }

    template <typename CharT = char>
    operator const CharT*() {
        return reinterpret_cast<const CharT*>(ZSTR_VAL(getOrEmpty()));
    }

    String& makeWritable() {
        // An empty string is "writeable" for a length of zero.
        // Silly but it's true.
        if (!_str) {
            _str = zend_string_init("", 0, 0);
            return *this;
        }

        if (GC_REFCOUNT(_str) == 1) return *this;

        auto *newstr = zend_string_init(val(), len(), 0);
        zend_string_release(_str);
        _str = newstr;
        return *this;
    }

    // Non-const mutators must have a real underlying string to return.
    template <typename CharT = char>
    CharT* val() { return _str ? reinterpret_cast<CharT*>(ZSTR_VAL(_str)) : nullptr; }
    size_t* lenPtr() { return &ZSTR_LEN(_str); }

    template <typename CharT>
    operator CharT*() {
        return reinterpret_cast<CharT*>(ZSTR_VAL(_str));
    }

    String& terminate() {
        // Technically this shouldn't be done to a not "isWritable",
        // but null termination should also be expected regardless.
        ZSTR_VAL(_str)[ZSTR_LEN(_str)] = 0;
        return *this;
    }

    // Direct pointer accessors.
    zend_string* get() { return _str; }
    const zend_string* getOrEmpty() const { return _str ? _str : zend_empty_string; }
    zend_string* release() {
        auto ret = _str;
        _str = nullptr;
        return ret;
    }
    zend_string* addref() {
        zend_string_addref(_str);
        return _str;
    }
private:
    zend_string *_str;
};

}  // namespace Zeval
