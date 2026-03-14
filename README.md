# PHP OQS Extension

This extension wraps [liboqs](https://github.com/open-quantum-safe/liboqs).
It provides two primary classes for [Key Encapsulation Methods](poqs-kem.stub.php)
and [Digital Signatures](poqs-sig.stub.php).

## Examples

### Key Encapsulation Methods (KEM)

The KEM encaps method is designed to generate cryptographically secure random
"shared secret" cipher keys which may be used with common symmetric methods
such as AES256.
These cipher keys are also "encapsulated" in a ciphertext encrypted using the
KEM keypair generated at object creation time.  This ciphertext may be unencapsulated
by a later process with access to the secret key portion of 

#### Generate a keypair.

In the generated keypair, any system which needs to generate an encapsulated key
will require the `public` portion of the KEM key, while any system which needs to
unencapsulate that key will require the `secret` portion of the KEM key.

```php
<?php

// Instantiating with just a method name will generate a new, unique keypair.
$kem = new \POQS\KEM("ML-KEM-512");

// Save public key in an appropriate place to use for later encaps operations.
$public = $kem->getPublicKey();
file_put_contents(__DIR__ . "/examples/scratch/ml-kem-512-key64.public", base64_encode($public));

// Save secret key in an appropriate place to use for later decaps operations.
$secret = $kem->getSecretKey();
file_put_contents(__DIR__ . "/examples/scratch/ml-kem-512-key64.secret", base64_encode($secret));

unset($kem, $public, $secret);
?>
```

#### Generate a cipher key, encrypt some data, and store it with the encapsulated cipher key.

Once you have a keypair, the public key component of it may be used to generate
a cipher key for use with encryption, and immediately encapsulate it for later use.

The "shared secret" may be immediately used for encryption, or recreated later from the
"ciphertext" of the encapsulated key, but the "shared secret" must not be directly persisted.

Note here that we store openssl encrypt payloads with the randomly generated IV used
as is standard in symmetric encryption. This is not specific to OQS key encapsulation.

```
<?php
$kem = new \POQS\KEM("ML-KEM-512",
    base64_decode(file_get_contents(__DIR__ . "/examples/scratch/ml-kem-512-key64.public")),
    null /* secret key not required for encapsulation */);

// Generate a cipher key from a random seed, and encapsulate it using the KEM keypair.
['ciphertext' => $encaps_key, 'shared_secret' => $shared_secret] = $kem->encaps();

// Store the encapsulated cipher key for later use.
file_put_contents(__DIR__ . "/examples/scratch/encrypted-data.cipherkey.encrypted.b64", base64_encode($encaps_key));

// Encrypt a block of data using the `shared_secret' with openssl or similar...
$iv = random_bytes(openssl_cipher_iv_length('aes-128-cbc'));
$encrypted = openssl_encrypt("The quick brown fox jumped over the lazy dog.", "aes-128-cbc", $shared_secret, 0, $iv);
file_put_contents(__DIR__ . "/examples/scratch/encrypted-data.pem", base64_encode($iv . $encrypted));

unset($kem, $encaps_key, $shared_secret, $iv, $encrypted);
?>
```

#### Use the encapsulated cipher key to decrypt data later.

Recreate the "shared secret" key from the encapsulated key using the generated (and secured)
KEM secret key.

We can use this "shared secret" to perform ordinary decryption of the encrypted payload,
with its bundled IV, to retrieve the original plaintext message.

```
<?php

$kem = new \POQS\KEM("ML-KEM-512",
    null /* public key not required for encapsulation */,
    base64_decode(file_get_contents(__DIR__ . "/examples/scratch/ml-kem-512-key64.secret")));


// Unwrap the encapsulated key.
$shared_secret = $kem->decaps(
    base64_decode(file_get_contents(__DIR__ . "/examples/scratch/encrypted-data.cipherkey.encrypted.b64")));

// Decrypt the data.
$iv_and_ciphertext = base64_decode(file_get_contents(__DIR__ . "/examples/scratch/encrypted-data.pem"));
$ivlen = openssl_cipher_iv_length('aes-128-cbc');
$message = openssl_decrypt(
	substr($iv_and_ciphertext, $ivlen),
    "aes-128-cbc",
    $shared_secret,
	0 /* options */,
	substr($iv_and_ciphertext, 0, $ivlen));

echo "The original message is: $message\n";

unset($kem, $iv_and_ciphertext, $ivlen, $shared_secret, $message);
?>
```
