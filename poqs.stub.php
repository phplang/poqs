<?php

/**
 * @generate-function-entries
 * @generate-class-entries
 */

namespace POQS {

class ErrorException extends \ErrorException {}
class ValueError extends \ValueError {}

/**
 * Version of library currently loaded via dynamic linking.
 *
 * @var string
 * @cvalue POQS_OQS_VERSION
 */
const VERSION = UNKNOWN;

/**
 * Version of headers compiled against.
 *
 * @var string
 * @cvalue OQS_VERSION_TEXT
 */
const VERSION_COMPILED = UNKNOWN;

} // namespace POQS
