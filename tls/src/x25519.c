#include "crypt.h"
#include "fe51.h"
#include "log.h"
#include "utils.h"

#include <string.h>

#define ALL_ZEROS 0x0000000000000000ULL
#define ALL_ONES 0xFFFFFFFFFFFFFFFFULL

#define A 486662
#define A24 ((A - 2) / 4)

#define BITS 255
#define UP 9
#define C19 19

static u64 CHUNKS[4] = {0};

void encode_u_coordinate(u8 *bytes, fe51 field_elem) {
    /* return int(u % P).to_bytes(32, "little") */
    reduce(field_elem);
    memset(CHUNKS, 0, 4);
    unpack_values(CHUNKS, field_elem);

#pragma unroll
    for (u8 i = 0; i < 4; i++) { u64_to_bytes(CHUNKS[i], bytes + (u64)(i * ONE_BYTE)); }
}

void reduce(fe51 field_elem) {
    carry_propagate(field_elem);
    carry_propagate(field_elem);

    u64 carry = (field_elem[0] + PRIME) >> LIMB_SIZE;
    carry     = (field_elem[1] + carry) >> LIMB_SIZE;
    carry     = (field_elem[2] + carry) >> LIMB_SIZE;
    carry     = (field_elem[3] + carry) >> LIMB_SIZE;
    carry     = (field_elem[4] + carry) >> LIMB_SIZE;

    field_elem[0] += PRIME * carry;
    field_elem[1] += (field_elem[0] >> LIMB_SIZE);

    field_elem[0] &= MASK51;
    field_elem[2] += (field_elem[1] >> LIMB_SIZE);

    field_elem[1] &= MASK51;
    field_elem[3] += (field_elem[2] >> LIMB_SIZE);

    field_elem[2] &= MASK51;
    field_elem[4] += (field_elem[3] >> LIMB_SIZE);

    field_elem[3] &= MASK51;
    field_elem[4] &= MASK51;
}

void carry_propagate(fe51 field_elem) {
    u64 carry = field_elem[0] >> LIMB_SIZE;
    field_elem[0] &= MASK51;
    field_elem[1] += carry;

    carry = field_elem[1] >> LIMB_SIZE;
    field_elem[1] &= MASK51;
    field_elem[2] += carry;

    carry = field_elem[2] >> LIMB_SIZE;
    field_elem[2] &= MASK51;
    field_elem[3] += carry;

    carry = field_elem[3] >> LIMB_SIZE;
    field_elem[3] &= MASK51;
    field_elem[4] += carry;

    // Special step
    carry = field_elem[4] >> LIMB_SIZE;
    field_elem[4] &= MASK51;
    field_elem[0] += carry * C19;
}

void pack_values(const u64 vals[4], fe51 field_elem) {
    u64 steps_right[] = {LIMB_SIZE, (LIMB_SIZE - LIMB_STEP), (LIMB_SIZE - (2 * LIMB_STEP))};
    u64 steps_left[]  = {LIMB_STEP, (LIMB_STEP * 2), (LIMB_STEP * 3)};

    LOG_INFO("In pack_values vals: {%lu, %lu, %lu, %lu}", vals[0], vals[1], vals[2], vals[3]);
    field_elem[0] = vals[0] & MASK51;

#pragma unroll
    for (u8 i = 1; i < 4; i++) { field_elem[i] = ((vals[i - 1] >> steps_right[i - 1]) | (vals[i] << steps_left[i - 1])) & MASK51; }
    field_elem[4] = ((vals[3] >> (LIMB_SIZE - (LIMB_STEP * 3)))) & MASK51;
}

void unpack_values(u64 vals[4], const fe51 field_elem) {
    u64 steps_left[]  = {(LIMB_SIZE - LIMB_STEP), (LIMB_SIZE - (2 * LIMB_STEP)), (LIMB_SIZE - (3 * LIMB_STEP))};
    u64 steps_right[] = {LIMB_STEP, (LIMB_STEP * 2), (LIMB_STEP * 3)};

    vals[0] = field_elem[0] | (field_elem[1] << LIMB_SIZE);

#pragma unroll
    for (u8 i = 1; i < 4; i++) { vals[i] = ((field_elem[i] >> steps_right[i - 1]) | (field_elem[i + 1] << steps_left[i - 1])); }
}

void decode_u_coordinate(fe51 field_elem, const u8 bytes[BYTES]) {
    u8 byte_slice[ONE_BYTE] = {0}; // clang-format off
    u64 vals[4] = {0}; // clang-format on

#pragma unroll
    for (u8 i = 0; i < 4; i++) {
        memcpy(byte_slice, bytes + (u64)(i * ONE_BYTE), ONE_BYTE);
        vals[i] = u64_from_bytes_le(byte_slice);
    }

    LOG_INFO("After loop vals: {%lu, %lu, %lu, %lu}", vals[0], vals[1], vals[2], vals[3]);
    pack_values(vals, field_elem);
}

/*
   cswap(swap, x_2, x_3):
         dummy = mask(swap) AND (x_2 XOR x_3)
         x_2 = x_2 XOR dummy
         x_3 = x_3 XOR dummy
         Return (x_2, x_3)

   Where mask(swap) is the all-1 or all-0 word of the same length as x_2
   and x_3, computed, e.g., as mask(swap) = 0 - swap.*/

void cswap(u64 swap, u64 *x_coords) {
    u64 mask    = 0 - swap;
    u64 dummy   = mask & (x_coords[1] ^ x_coords[2]);
    x_coords[1] = x_coords[1] ^ dummy;
    x_coords[2] = x_coords[2] ^ dummy;
}

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
u64 curve25519(fe51 scalar_k, fe51 u_coord) {
    fe51 a24 = {A24};
    return 0;
}
