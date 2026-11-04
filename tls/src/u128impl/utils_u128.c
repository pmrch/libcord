#if !defined(__SIZEOF_INT128__)

#include "types.h"
#include "utils.h"

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
u128 u128_add(u128 left, u128 right) {
    u128 result = {0};

    unsigned char carry = _addcarry_u64(0, left.low_bits, right.low_bits, &result.low_bits);
    _addcarry_u64(carry, left.high_bits, right.high_bits, &result.high_bits);
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

#else

#ifndef _MSC_VER
__attribute__((unused)) static void dont_flag(void) {}
#endif

#endif
