#ifndef CRYPT_H
#define CRYPT_H

#include "utils.h"

#define BYTES 32
#define MASK51 0x7ffffffffffffULL

#define MUL_PROPAGATE(a, b, out) \
    fe_mul(a, out, (b))

#define SUB_PROPAGATE(a, b, out) \
    fe_sub(a, out, (b));           \
    carry_propagate(out)

void pack_values(const u64 vals[4], fe51 field_elem);
void unpack_values(u64 vals[4], const fe51 field_elem);

void carry_propagate(fe51 field_elem);
void reduce(fe51 field_elem);

void decode_u_coordinate(fe51 field_elem, const u8 bytes[BYTES]);
void encode_u_coordinate(u8 *bytes, fe51 field_elem);

void clamp_scalar(u8 *restrict bytes);
void curve25519(u8 *scalar_bytes, u8 *u_coord, u64 *restrict result);

#endif
