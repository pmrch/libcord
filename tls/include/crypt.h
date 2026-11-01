#ifndef CRYPT_H
#define CRYPT_H

#include "types.h"
#include "fe51.h"

void decode_u_coordinate(fe51 field_elem, const u8 *bytes);
void encode_u_coordinate(u8 *restrict bytes, u64 *restrict field_elem);

void curve25519(u8 *scalar_bytes, u8 *u_coord, u64 *restrict result);

#endif
