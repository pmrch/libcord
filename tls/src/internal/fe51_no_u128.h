#if !defined(FE51_NO_U128_H)
#define FE51_NO_U128_H

#if !defined(__SIZEOF_INT128__)
#include "platform.h"
#include "types.h"

static inline u64 u128_high(u128 num) { return num.high_bits; }
static inline u64 u128_low(u128 num) { return num.low_bits; }

static inline u64 u128_shr(u128 num, u8 shift) {
    (void)shift;    
    return (num.low_bits >> LIMB_SIZE) | (num.high_bits << (BITS64 - LIMB_SIZE));
}

// NOLINTNEXTLINE (bugprone-easily-swappable-parameters)
static inline u128 mul_u64(u64 left, u64 right) {
    u128 result;
    result.low_bits = _umul128(left, right, &result.high_bits);
    return result;
}

#endif

#endif
