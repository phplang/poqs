#pragma once

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"

namespace poqs {

ZEND_BEGIN_MODULE_GLOBALS(poqs)
    zend_string *prng_data;
    size_t prng_pos;
ZEND_END_MODULE_GLOBALS(poqs)

PHPAPI ZEND_EXTERN_MODULE_GLOBALS(poqs);
#define POQSG(v) ZEND_MODULE_GLOBALS_ACCESSOR(poqs, v)

#if defined(ZTS) && defined(COMPILE_DL_POQS)
ZEND_TSRMLS_CACHE_EXTERN()
#endif

}  // namespace poqs
