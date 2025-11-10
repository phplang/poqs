<?php declare(strict_types=1);

#[Attribute(Attribute::TARGET_PROPERTY)]
class POQSHexProp {
    static public function shortHex(string $bin): string {
        if (strlen($bin) <= 16) {
            return bin2hex($bin);
        }
        return bin2hex(substr($bin, 0, 8)) . '...' . bin2hex(substr($bin, -8));
    }
}

#[Attribute(Attribute::TARGET_PROPERTY)]
class POQSCountProp {}

// Treat a given value the same as null.
#[Attribute(Attribute::TARGET_PROPERTY)]
class POQSNullableProp {
    public function __construct(public readonly string $none = 'none') {}
}

class ACVPTest {
    const DEBUG = true;

    public function __construct(
        public readonly ACVPTestGroup $testGroup,
        public readonly int $tcId,
        public readonly bool $testPassed = true,
        public readonly bool $deferred = false,

        #[POQSHexProp]
        public readonly ?string $seed = null,
        #[POQSHexProp]
        public readonly ?string $pkSeed = null,
        #[POQSHexProp]
        public readonly ?string $skSeed = null,
        #[POQSHexProp]
        public readonly ?string $d = null,
        #[POQSHexProp]
        public readonly ?string $z = null,
        #[POQSHexProp]
        public readonly ?string $pk = null,
        #[POQSHexProp]
        public readonly ?string $sk = null,
        #[POQSHexProp]
        public readonly ?string $ek = null,
        #[POQSHexProp]
        public readonly ?string $dk = null,
        #[POQSHexProp]
        public readonly ?string $rnd = null,
        #[POQSHexProp]
        public readonly ?string $additionalRandomness = null,
        #[POQSHexProp]
        public readonly ?string $message = null,
        #[POQSHexProp]
        public readonly ?string $context = null,
        #[POQSHexProp]
        public readonly ?string $signature = null,
        #[POQSNullableProp('none')]
        public readonly ?string $hashAlg = null,
        #[POQSHexProp]
        public readonly ?string $m = null,
        #[POQSHexProp]
        public readonly ?string $c = null,
        #[POQSHexProp]
        public readonly ?string $k = null,
    ) {}

    static private function getAlgoClass(string $method): string {
        if (in_array($method, \POQS\KEM::algos())) {
            return \POQS\KEM::class;
        } elseif (in_array($method, \POQS\SIG::algos())) {
            return \POQS\SIG::class;
        } else {
            throw new \Exception("Unknown method '$method'");
        }
    }

    protected function doAssert(mixed $a, mixed $b, bool $shouldPass, string $context, ?callable $transform) : void {
        if ($shouldPass !== ($a === $b)) {
            $error = $shouldPass ? "Test failed" : "Test passed when it should have failed";
            $equality = $shouldPass ? "!==" : "===";
            if ($context !== '') $error .= ", $context";
            if ($transform === null) {
                $transform = fn($v) => ($v === true) ? 'true' : (($v === false) ? 'false' : print_r($v, true));
            }
            if (static::DEBUG) {
                var_dump($this, $this->testGroup, $this->testGroup->testVector);
            }
            throw new \Exception("$error: {$transform($a)} $equality {$transform($b)}");
        }
    }

    public function assertSame(mixed $a, mixed $b, string $context = '', ?callable $transform = null) : void {
        $this->doAssert($a, $b, $this->testPassed, $context, $transform);
    }

    public function assertSameBinary(mixed $a, mixed $b, string $context = '') : void {
        $this->assertSame($a, $b, $context, \POQSHexProp::shortHex(...));
    }

    public function assertRevision(string $method, string $revision): void {
        $cls = static::getAlgoClass($method);
        $this->doAssert($cls::getParameters($method)['alg_version'],
                                            $revision,
                                            true, /* shouldPass */
                                            "Revision for '$method'",
                                            null /* transform */);
    }

    public function getEntropySource(): ?string {
        $group = $this->testGroup;
        if ($group->isSLHDSA) {
            $method = $group->transformedParameterSet;
            $rnd = $this->additionalRandomness ?? null;
            if ($rnd === null) {
                // This is an absolutely pathological way to encode rng data, brah.
                $lpk = \POQS\SIG::getParameters($method)['length_public_key'];
                $rnd = substr($this->sk, $lpk, $lpk);
            }
            return $rnd;
        }

        if ($group->isMLDSA) {
            return $this->rnd ?? ($group->deterministic ? str_repeat("\0", 64) : null);
        }
        throw new \Exception("Unknown sigGen test method prefix: {$group->parameterSet}");
    }

    public function runKeyGenTest(): void {
        $group = $this->testGroup;
        $method = $group->transformedParameterSet;

        $seed = $group->isSLHDSA ? $this->sk : ($this->seed ?? ($this->d . $this->z));
        $alg = match (static::getAlgoClass($method)) {
            \POQS\KEM::class => \POQS\KEM::newFromSeed($method, $seed),
            \POQS\SIG::class => (function() use ($method, $seed) {
                // Only KEM has a newWithSeed, SIG does not. :shrug:
                \POQS\PRNG::setData($seed);
                $alg = new \POQS\SIG($method);
                \POQS\PRNG::useSystem();
                return $alg;
            })(),
        };

        $this->assertSameBinary($alg->getPublicKey(), $this->pk ?? $this->ek, "public key");
        $this->assertSameBinary($alg->getSecretKey(), $this->sk ?? $this->dk, "secret key");
    }

    public function runSigGenVerTest(bool $verify): void {
        $group = $this->testGroup;

        // Ignore 'internal' tests as we're not testing the internals of liboqs,
        // we're only testing that we've bound the external interface correctly.
        if ($group->signatureInterface !== 'external') { return; }

        // TODO: How are preHashes handled?
        if ($group->preHash !== 'pure') { return; }

        $rnd = $this->getEntropySource();
        if ($rnd !== null) {
            \POQS\PRNG::setData($rnd);
        }

        try {
            $method = $group->transformedParameterSet;
            $alg = new \POQS\SIG($method, $this->pk, $this->sk);
            $message = $this->message;
            if ($this->hashAlg !== null) {
                $message = hash(strtolower($this->hashAlg), $message, true);
            }
            if ($verify) {
                $this->assertSame(true, $alg->verify($message, $this->signature, $this->context), "verify signature");
            } else {
                $signature = $alg->sign($message, $this->context);
                $this->assertSameBinary($signature, $this->signature, "generate signature");
            }
        } finally {
            if ($rnd !== null) {
                \POQS\PRNG::useSystem();
            }
        }
    }

    public function runEncapsulationTest(): void {
        $group = $this->testGroup;
        $alg = new \POQS\KEM($group->parameterSet, $this->ek, $this->dk);
        $enc = $alg->encaps($this->m);
        $this->assertSameBinary($enc['ciphertext'],    $this->c, "encaps ciphertext");
        $this->assertSameBinary($enc['shared_secret'], $this->k, "encaps shared_secret");
    }

    public function runDecapsulationTest(): void {
        $group = $this->testGroup;
        $alg = new \POQS\KEM($group->parameterSet, $this->ek, $this->dk);
        $key = $alg->decaps($this->c);
        $this->assertSameBinary($key, $this->k, "decaps");
    }

    public function runEnDecapTest(): void {
        match ($this->testGroup->function) {
            'encapsulation' => $this->runEncapsulationTest(),
            'decapsulation' => $this->runDecapsulationTest(),
            'encapsulationKeyCheck' => null, /* ignored? */
            'decapsulationKeyCheck' => null, /* ignored? */
        };
    }

    public function runTest(): void {
        $group = $this->testGroup;
        try {
            $this->assertRevision($group->transformedParameterSet, $group->testVector->revision);
               match ($group->testVector->mode) {
                'keyGen' => $this->runKeyGenTest(),
                'sigGen' => $this->runSigGenVerTest(false /* generate only */),
                'sigVer' => $this->runSigGenVerTest(true /* verify only */),
                'encapDecap' => $this->runEnDecapTest(),
                default => throw new \Exception("Unknown mode $mode"),
            };
        } catch (\Exception $ex) {
            $mode = $group->testVector->mode;
            $tgId = $group->tgId;
            $tcId = $this->tcId;
            throw new Exception("Failed running {$mode} test tgId: $tgId, tcId: $tcId", 0, $ex);
        }
    }

    static public function newFromJSON(ACVPTestGroup $group, object $data) : \ACVPTest {
        $args = ['testGroup' => $group];
        foreach ($data as $prop => $value) {
            if (!property_exists(\ACVPTest::class, $prop)) continue;
            $attrs = (new ReflectionProperty(\ACVPTest::class, $prop))->getAttributes();
            foreach ($attrs as $attr) {
				$value = match ($attr->getName()) {
                    'POQSHexProp' => hex2bin($value),
                    'POQSNullableProp' => ($value === $attr->newInstance()->none) ? null : $value,
					default => $value,
                };
            }
            $args[$prop] = $value;
        }

        return new \ACVPTest(...$args);
    }

    public function __debugInfo(): array { return ACVPTestVector::debugInfoHelper($this); }
}


class ACVPTestGroup {
    public readonly bool $isMLDSA;
    public readonly bool $isSLHDSA;
    public readonly string $transformedParameterSet;

    public function __construct(
        public readonly ACVPTestVector $testVector,     # Back reference
        public readonly int $tgId,                      # TestGroup ID
        public readonly string $testType,               # Test Type (e.g. "AFT")
        public readonly string $parameterSet,           # Sepcific algorithm
        public readonly ?string $signatureInterface,    # ? (e.g. "external")
        public readonly ?string $preHash,               # ? (e.g. "pure")
        public readonly ?bool $deterministic,           # The same inputs always produce the same outputs.
        public readonly ?bool $externalMu,              # ?
        public readonly ?string $cornerCase,            # ? (e.g. "none")
        public readonly ?string $function,              # Operation (e.g. "encapsulate", "decapsulate", etc...)
        #[POQSCountProp]
        public array $tests = [] { get => $this->tests; }  # array<ACVPTest>
    ) {
        $this->isMLDSA = static::isMLDSA($parameterSet);
        $this->isSLHDSA = static::isSLHDSA($parameterSet);
        $this->transformedParameterSet = static::transformSLHDSAName($parameterSet);
    }

    static public function isSLHDSA(string $method): bool {
        return 0 == strncmp($method, "SLH-DSA-", 8);
    }

    static public function isMLDSA(string $method): bool {
        return 0 == strncmp($method, "ML-DSA-", 7);
    }

    public function transformSLHDSAName(string $method): string {
        if  (!static::isSLHDSA($method)) return $method;

        $method = str_replace('-', '_', $method);
        $method[strlen($method)-1] = strtoupper($method[strlen($method)-1]);
        if (($this->testGroup->preHash ?? '') === 'preHash') {
            $method = 'SLH_DSA_' . str_replace('-', '_', $this->hashAlg) . '_PREHASH' . substr($method, 7);
        } else {
            $method = 'SLH_DSA_PURE' . substr($method, 7);
        }

        return $method;
    }

    public function runAllTestCases(): void {
        foreach ($this->tests as $test) {
            $test->runTest();
        }
    }

    static public function newFromJSON(ACVPTestVector $tv, object $data) : \ACVPTestGroup {
        $tg = new \ACVPTestGroup($tv, $data->tgId, $data->testType,
            $data->parameterSet, $data->signatureInterface ?? null,
            $data->preHash ?? null, $data->deterministic ?? null,
            $data->externalMu ?? null, $data->cornerCase ?? null,
            $data->function ?? null);

        $tests = $tg->tests;
        foreach ($data->tests as $test) {
            $tests[] = \ACVPTest::newFromJSON($tg, $test);
        }
        $tg->tests = $tests;

        return $tg;
    }

    public function __debugInfo() : array { return ACVPTestVector::debugInfoHelper($this); }
}

class ACVPTestVector {
    public function __construct(
        public readonly int $vsId,             # VectorSet ID
        public readonly string $algorithm,     # Algorithm group, see testGroup[].parameterSet for specific algorithm
        public readonly string $mode,          # Test mode (e.g. "keyGen" / "sigVer"
        public readonly string $revision,      #
        public readonly bool $isSample,        #
        #[POQSCountProp]
        public array $testGroups = [] { get => $this->testGroups; }, # array<\ACVPTestGroup>
    ) {}

    public function runAllTestGroups(): void {
        foreach ($this->testGroups as $group) {
            $group->runAllTestCases();
        }
    }

    static function parseFile(string $filename): \ACVPTestVector {
        $data = json_decode(file_get_contents($filename));

        $v = new ACVPTestVector($data->vsId, $data->algorithm, $data->mode,
                $data->revision, $data->isSample);

        $groups = $v->testGroups;
        foreach ($data->testGroups as $tg) {
            $groups[] = \ACVPTestGroup::newFromJSON($v, $tg);
        }
        $v->testGroups = $groups;

        return $v;
    }

    public function __debugInfo(): array { return ACVPTestVector::debugInfoHelper($this); }

    static public function debugInfoHelper(object $obj): array {
        $props = (array)$obj;
        foreach ($props as $k => &$v) {
            if ($v === null) continue;

            $attrs = new ReflectionProperty($obj, $k)->getAttributes();
            foreach ($attrs as $attr) {
                switch ($attr->getName()) {
                    case 'POQSHexProp':   $v = \POQSHexProp::shortHex($v); break;
                    case 'POQSCountProp': $v = count($v);            break;
                }
            }

            if ($v instanceof \ACVPTestGroup) {
                $v = "{$v->tgId}-{$v->parameterSet}-{$v->signatureInterface}-{$v->preHash}";
            } elseif ($v instanceof \ACVPTestVector) {
                $v = "{$v->vsId}-{$v->algorithm}-{$v->mode}";
            }
        }
        return $props;
    }
}
