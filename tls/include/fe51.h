#ifndef FE51_H
#define FE51_H

#include "crypt.h"
#include "utils.h"

#ifndef HAS_UINT128
#include "fe51_no_u128.h"
#endif

#define PRIME 19ULL

#define p_01 0xFFFFFFFFFFFDAULL
#define P 0xFFFFFFFFFFFFEUL

#ifdef HAS_UINT128
static inline void mul64(u64 term0, u128 *out, u64 term1) { *out = ((u128)term0 * (u128)term1); }
static inline u128 u128_shr(u128 num, const u8 SHIFT) { return num >> SHIFT; }

static inline u64 u128_shr64(u128 num, const u8 SHIFT) { return (u64)(num >> SHIFT); }
static inline u64 u128_high(u128 num) { return (u64)(num >> BITS64); }
static inline u64 u128_low(u128 num) { return (u64)num; }
#endif

// NOLINTBEGIN(readability-identifier-length)
void fe_mul(const u64 *restrict a, u64 *restrict result, const u64 *restrict b);

void fe_pow22501(const fe51 z_2, fe51 *out);
void fe_invert(fe51 out, const fe51 input);
void fe_sq(u64 *out, const u64 *a);

void fe_frombytes(const u8 *restrict bytes, u64 *restrict output);

// Implementations
static inline void fe_add(const u64 *restrict a, u64 *restrict result, const u64 *restrict b) {
    for (u8 i = 0; i < LIMBS; i++) { result[i] = a[i] + b[i]; }

    // Carry propagation for the top limbs
    result[0] += (result[4] >> LIMB_SIZE) * PRIME;
    result[4] &= MASK51;
}

static inline void fe_sub(const u64 *restrict a, u64 *restrict result, const u64 *restrict b) {
    result[0] = a[0] + p_01 - b[0];
    for (u8 i = 1; i < LIMBS; i++) { result[i] = (a[i] + P) - b[i]; }

    u64 carry = result[4] >> LIMB_SIZE;
    result[0] += carry * PRIME;
    result[4] &= MASK51;
}

// NOLINTEND(readability-identifier-length)

#endif
