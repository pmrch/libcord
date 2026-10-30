#include "crypt.h"
#include "fe51.h"
#include "utils.h"

#ifndef HAS_UINT128

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
u128 u128_add(u128 left, u128 right) {
    pseudo_u128 result = {0};

    result.low_bits  = left.low_bits + right.low_bits;
    result.high_bits = left.high_bits + right.high_bits + (result.low_bits < left.low_bits ? 1 : 0);

    return result;
}

u128 u128_add_u64(u128 num_a, u64 num_b) {
    pseudo_u128 result = {0};

    result.low_bits  = num_a.low_bits + num_b;
    result.high_bits = num_a.high_bits + (result.low_bits < num_a.low_bits ? 1 : 0);

    return result;
}

u128 u128_shr51(u128 num_a) {
    pseudo_u128 result = {0};

    result.low_bits  = (num_a.low_bits >> LIMB_SIZE) | (num_a.high_bits << LIMB_STEP);
    result.high_bits = num_a.high_bits >> LIMB_SIZE;
    return result;
}

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
u128 mul_u64(u64 left, u64 right) {
    u64 a_low = left & MASK32;
    u64 b_low = right & MASK32;

    u64 a_high = left >> SHIFT32;
    u64 b_high = right >> SHIFT32;

    u64 low_low   = a_low * b_low;
    u64 low_high  = a_low * b_high;
    u64 high_low  = a_high * b_low;
    u64 high_high = a_high * b_high;

    // mid is the cross terms combined
    // clang-format off
    u64 mid = (low_low >> SHIFT32) + (low_high & MASK32) + high_low;
    u128 result = {
        .low_bits  = (low_low & MASK32) | (mid << SHIFT32),
        .high_bits = high_high + (low_high >> SHIFT32) + (mid >> SHIFT32)
    }; // clang-format on

    return result;
}

void map_terms_to_limbs(u128 terms[LIMBS], fe51 result) {
    u128 carry = {0};

    result[0] = terms[0].low_bits & MASK51;
    carry     = u128_shr51(terms[0]);
    terms[1]  = u128_add(terms[1], conv);

    result[1] = terms[1].low_bits & MASK51;
    carry     = u128_shr51(terms[1]);
    terms[2]  = u128_add(terms[2], conv);

    result[2] = terms[2].low_bits & MASK51;
    carry     = u128_shr51(terms[2]);
    terms[3]  = u128_add(terms[3], conv);

    result[3] = terms[3].low_bits & MASK51;
    carry     = u128_shr51(terms[3]);
    terms[4]  = u128_add(terms[4], conv);

    result[4] = terms[4].low_bits & MASK51;
    carry     = u128_shr51(terms[4]);

    u128 c19 = mul_u64(conv.low_bits, PRIME);
    result[0] += c19.low_bits;

    // THE FIX: We must propagate the carry from result[0] before we return!
    // Otherwise, result[0] might be > 2^51 when we exit.
    result[1] += result[0] >> LIMB_SIZE;
    result[0] &= MASK51;
}

#else
void map_terms_to_limbs(u128 *restrict terms, u64 *restrict result) {
    result[0] = (u64)(terms[0]) & MASK51;
    terms[1] += terms[0] >> LIMB_SIZE;

    result[1] = (u64)(terms[1]) & MASK51;
    terms[2] += terms[1] >> LIMB_SIZE;

    result[2] = (u64)(terms[2]) & MASK51;
    terms[3] += terms[2] >> LIMB_SIZE;

    result[3] = (u64)(terms[3]) & MASK51;
    terms[4] += terms[3] >> LIMB_SIZE;

    result[4] = (u64)(terms[4]) & MASK51;
    u64 carry = (u64)(terms[4] >> LIMB_SIZE);

    result[0] += (carry * PRIME);

    // THE FIX: We must propagate the carry from result[0] before we return!
    // Otherwise, result[0] might be > 2^51 when we exit.
    result[1] += result[0] >> LIMB_SIZE;
    result[0] &= MASK51;
}

#endif
