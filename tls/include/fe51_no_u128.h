#ifndef FE51_NO_U128_H
#define FE51_NO_U128_H

#include "utils.h"

static inline pseudo_u128 shr128(pseudo_u128 num, unsigned shift) {
    pseudo_u128 result;
    result.low_bits  = (num.low_bits >> shift) | (num.high_bits << (BITS64 - shift));
    result.high_bits = num.high_bits >> shift;

    return result;
}

static inline void mul64(u64 term1, pseudo_u128 *out, u64 term2) {
    pseudo_u128 term_a = {.low_bits = term1 & MASK32, .high_bits = term1 >> BITS32};
    pseudo_u128 term_b = {.low_bits = term2 & MASK32, .high_bits = term2 >> BITS32};

    // clang-format off
    Helper4u64 products = {
        .t0 = term_a.low_bits * term_b.low_bits, .t1 = term_a.low_bits * term_b.high_bits,
        .t2 = term_a.high_bits * term_b.low_bits, .t3 = term_a.high_bits * term_b.high_bits
    }; // clang-format on

    u64 mid = (products.t0 >> BITS32) + (products.t1 & MASK32) + (products.t2 & MASK32);

    out->low_bits  = (products.t0 & MASK32) | (mid << BITS32);
    out->high_bits = (products.t1 >> BITS32) + (products.t2 >> BITS32) + (mid >> BITS32);
}

#endif
