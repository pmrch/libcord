#ifndef CRYPT_H
#define CRYPT_H

#include "types.h"

void pack_values(const u64 *restrict vals, u64 *restrict field_elem);
void unpack_values(u64 *restrict vals, const u64 *restrict field_elem);

void carry_propagate(fe51 field_elem);
void reduce(u64 *restrict field_elem);

void decode_u_coordinate(fe51 field_elem, const u8 *bytes);
void encode_u_coordinate(u8 *restrict bytes, u64 *restrict field_elem);

void clamp_scalar(u8 *restrict bytes);
void curve25519(u8 *scalar_bytes, u8 *u_coord, u64 *restrict result);

#endif
