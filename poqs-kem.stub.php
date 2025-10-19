<?php

/**
 * @generate-function-entries
 * @generate-class-entries
 */

namespace POQS {

class KEM {
    public function __construct(string $method_name, ?string $public_key = null, ?string $secret_key = null) {}
    static public function newFromSeed(string $method_name, string $seed): \POQS\KEM {}

    public function getMethodName(): string {}
    public function getPublicKey(): ?string {}
    public function getSecretKey(): ?string {}

    public function encaps(string $shared_secret, ?string $seed = null): string {}
    public function decaps(string $ciphertext): string {}

    static public function algos(): array {}
    static public function getParameters(string $method_name): array {}
}

} // namespace POQS
