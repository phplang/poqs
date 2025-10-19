<?php

/**
 * @generate-function-entries
 * @generate-class-entries
 */

namespace POQS {

class SIG {
    public function __construct(string $method_name, ?string $public_key = null, ?string $secret_key = null) {}

    public function getMethodName(): string {}
    public function getPublicKey(): ?string {}
    public function getSecretKey(): ?string {}

    public function sign(string $message, ?string $context = null): string {}
    public function verify(string $message, string $signature, ?string $context = null): bool {}

    static public function algos(): array {}
    static public function getParameters(string $method_name): array {}
}

} // namespace POQS
