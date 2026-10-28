#ifndef CRYPT_H
#define CRYPT_H

#include "fe51.h"
#include "utils.h"

#define BYTES 32
#define MASK51 0x7ffffffffffffULL

void pack_values(const u64 vals[4], fe51 field_elem);
void unpack_values(u64 vals[4], const fe51 field_elem);

void carry_propagate(fe51 field_elem);
void reduce(fe51 field_elem);

void decode_u_coordinate(fe51 field_elem, const u8 bytes[BYTES]);
void encode_u_coordinate(u8 *bytes, fe51 field_elem);

u8 get_unicode_num(char chr);

#endif
