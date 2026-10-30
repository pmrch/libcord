#include "crypt.h"
#include "fe51.h"
#include "log.h"
#include "utils.h"

#include <string.h>
#include <sys/time.h>
#include <unistd.h>

#define ALL_ZEROS 0x0000000000000000ULL
#define ALL_ONES 0xFFFFFFFFFFFFFFFFULL

#define A 486662
#define A24 ((A - 2) / 4)

#define BITS 255
#define UP 9
#define C19 19

static u64 CHUNKS[4] = {0};

void encode_u_coordinate(u8 *bytes, fe51 field_elem) {
    reduce(field_elem);
    unpack_values(CHUNKS, field_elem);

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

    LOG_DEBUG("In pack_values vals: {%lu, %lu, %lu, %lu}", vals[0], vals[1], vals[2], vals[3]);
    field_elem[0] = vals[0] & MASK51;

    for (u8 i = 1; i < 4; i++) { field_elem[i] = ((vals[i - 1] >> steps_right[i - 1]) | (vals[i] << steps_left[i - 1])) & MASK51; }

    field_elem[4] = ((vals[3] >> (LIMB_SIZE - (LIMB_STEP * 3)))) & MASK51;
}

void unpack_values(u64 vals[4], const fe51 field_elem) {
    u64 steps_left[]  = {(LIMB_SIZE - LIMB_STEP), (LIMB_SIZE - (2 * LIMB_STEP)), (LIMB_SIZE - (3 * LIMB_STEP))};
    u64 steps_right[] = {LIMB_STEP, (LIMB_STEP * 2), (LIMB_STEP * 3)};

    vals[0] = field_elem[0] | (field_elem[1] << LIMB_SIZE);
    for (u8 i = 1; i < 4; i++) { vals[i] = ((field_elem[i] >> steps_right[i - 1]) | (field_elem[i + 1] << steps_left[i - 1])); }
}

void decode_u_coordinate(fe51 field_elem, const u8 bytes[BYTES]) {
    u8 byte_slice[ONE_BYTE] = {0}; // clang-format off
    memset(CHUNKS, 0, sizeof(CHUNKS));

    for (u8 i = 0; i < 4; i++) {
        memcpy(byte_slice, bytes + (u64)(i * ONE_BYTE), ONE_BYTE);
        CHUNKS[i] = u64_from_bytes_le(byte_slice);
    }

    LOG_DEBUG("After loop vals: {%lu, %lu, %lu, %lu}", CHUNKS[0], CHUNKS[1], CHUNKS[2], CHUNKS[3]);
    pack_values(CHUNKS, field_elem); // clang-format on
}

static inline void cswap(u64 *restrict x_2, u8 swap, u64 *restrict x_3) {
    u64 mask = 0 - ((u64)swap);
    for (u8 i = 0; i < LIMBS; i++) {
        u64 dummy = mask & (x_2[i] ^ x_3[i]);
        x_2[i] ^= dummy;
        x_3[i] ^= dummy;
    }
}

// NOLINTBEGIN(readability-magic-numbers)
void clamp_scalar(u8 *restrict bytes) {
    bytes[0] &= 248;
    bytes[31] &= 127;
    bytes[31] |= 64;
}
// NOLINTEND(readability-magic-numbers)
static u8 get_scalar_bit(const fe51 kurva, int32_t index) {
    int32_t limb = index / (int32_t)LIMB_SIZE;
    int32_t bit  = index % (int32_t)LIMB_SIZE;

    return (kurva[limb] >> bit) & 1;
}

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
void curve25519(u8 *scalar_bytes, u8 *u_coord, u64 *restrict result) {
    clamp_scalar(scalar_bytes);

    fe51 k_int = {0};
    fe51 x_1   = {0};
    fe51 x_3   = {0};

    fe_frombytes(scalar_bytes, k_int);
    decode_u_coordinate(x_1, u_coord);

    fe51 a24 = {A24, 0, 0, 0, 0};
    fe51 x_2 = {1, 0, 0, 0, 0};
    fe51 z_2 = {0, 0, 0, 0, 0};
    fe51 z_3 = {1, 0, 0, 0, 0};

    u8 swap = 0;
    u8 k_t  = 0;

    fe51 big_a  = {0};
    fe51 big_c  = {0};
    fe51 big_d  = {0};
    fe51 big_e  = {0};
    fe51 big_aa = {0};
    fe51 big_b  = {0};
    fe51 big_bb = {0};
    fe51 big_cb = {0};
    fe51 big_da = {0};

    fe51 mul_helper = {0};
    fe51 add_helper = {0};

    memcpy(x_3, x_1, sizeof(x_3)); // NOLINTNEXTLINE(readability-magic-numbers, altera-unroll-loops)
    for (int32_t i = 254; i >= 0; i--) {
        k_t = get_scalar_bit(k_int, i);
        swap ^= k_t;

        cswap(x_2, swap, x_3);
        cswap(z_2, swap, z_3);
        swap = k_t;

        // Montgomery ladder steps
        fe_add(x_2, big_a, z_2);
        fe_mul(big_a, big_aa, big_a);
        fe_sub(x_2, big_b, z_2);
        fe_mul(big_b, big_bb, big_b);

        fe_sub(big_aa, big_e, big_bb);
        fe_add(x_3, big_c, z_3);
        fe_sub(x_3, big_d, z_3);

        fe_mul(big_d, big_da, big_a);
        fe_mul(big_c, big_cb, big_b);

        fe_add(big_da, x_3, big_cb);
        fe_mul(x_3, x_3, x_3);
        fe_sub(big_da, z_3, big_cb);

        fe_mul(z_3, z_3, z_3);
        fe_mul(z_3, z_3, x_1);
        fe_mul(big_aa, x_2, big_bb);

        fe_mul(a24, mul_helper, big_e);
        fe_add(big_aa, add_helper, mul_helper);
        fe_mul(big_e, z_2, add_helper);
    }

    cswap(x_2, swap, x_3);
    cswap(z_2, swap, z_3);

    fe51 z_inv = {0};
    fe_invert(z_inv, z_2);
    MUL_PROPAGATE(x_2, z_inv, result);
}
