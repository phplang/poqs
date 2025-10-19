<?php

/**
 * @generate-function-entries
 * @generate-class-entries
 */

namespace POQS {

class PRNG {
    private function __construct() {}
    static public function getRemainingLength(): int {}
    static public function getRemainingData(): string {}
    static public function setData(string $data): void {}
    static public function useSystem(): void {}
    static public function useOpenSSL(): void {}
}

} // namespace POQS
