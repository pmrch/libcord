#ifndef CRYPT_H
#define CRYPT_H

#include "types.h"

void decode_u_coordinate(u64 *restrict field_elem, const u8 *restrict bytes);
void encode_u_coordinate(u8 *restrict bytes, u64 *restrict field_elem);

void curve25519(u8 *restrict scalar_bytes, const u8 *restrict u_coord, u64 *restrict result);

#endif
