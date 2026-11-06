#if !defined(FE51_NO_U128_H)
#define FE51_NO_U128_H

#if !defined(__SIZEOF_INT128__)
#include "platform.h"
#include "types.h"

static inline u64 u128_high(u128 num) { return num.high_bits; }
static inline u64 u128_low(u128 num) { return num.low_bits; }

static inline u128 u128_add(u128 left, u128 right) {
    u128 result = {0};

    unsigned char carry = _addcarry_u64(0, left.low_bits, right.low_bits, &result.low_bits);
    _addcarry_u64(carry, left.high_bits, right.high_bits, &result.high_bits);
    return result;
}

static inline u128 u128_add_u64(u128 num_a, u64 num_b) {
    pseudo_u128 result = {0};

    result.low_bits  = num_a.low_bits + num_b;
    result.high_bits = num_a.high_bits + (result.low_bits < num_a.low_bits ? 1 : 0);

    return result;
}

static inline u128 u128_shr51(u128 num_a) {
    pseudo_u128 result = {0};

    result.low_bits  = (num_a.low_bits >> LIMB_SIZE) | (num_a.high_bits << LIMB_STEP);
    result.high_bits = num_a.high_bits >> LIMB_SIZE;
    return result;
}

static inline u128 u128_shr(u128 num, u8 shift) {
    (void)shift;
    u128 result;
    result.low_bits  = (num.low_bits >> 51) | (num.high_bits << 13);
    result.high_bits = num.high_bits >> 51;

    return result;
}

// NOLINTNEXTLINE (bugprone-easily-swappable-parameters)
static inline u128 mul_u64(u64 left, u64 right) {
    u128 result;
    result.low_bits = _umul128(left, right, &result.high_bits);
    return result;
}

#endif

#endif
