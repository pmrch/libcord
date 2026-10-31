#ifndef CURVE25519_DONNA_H
#define CURVE25519_DONNA_H

#include <stdint.h>

/* Define u8 if not already defined */
#ifndef U8_DEFINED
#define U8_DEFINED
typedef uint8_t u8;
#endif

/**
 * Computes the Curve25519 public key or shared secret.
 *
 * @param mypublic   Output buffer for the 32-byte resulting point.
 * @param secret     Input buffer for the 32-byte private key/secret.
 * @param basepoint  Input buffer for the 32-byte basepoint (e.g., standard generator or public key).
 * @return           Returns 0 on success.
 */
int curve25519_donna(u8 *mypublic, const u8 *secret, const u8 *basepoint);

#endif /* CURVE25519_DONNA_H */
