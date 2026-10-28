#include "fe51.h"
#include "utils.h"

#define FELEN 5

#define p_01 0x7FFFFFFFFFFEDULL
#define P 0x7FFFFFFFFFFFFULL

// NOLINTBEGIN(readability-identifier-length)
void fe_add(const fe51 a, fe51 result, const fe51 b) {
UNROLL
    for (u8 i = 0; i < FELEN; i++) { result[0] = a[0] + b[0]; }
}

// The reason you add 2 * P is to prevent underflow you’re essentially adding a multiple of p (which doesn’t change the value mod p)
// to ensure the result stays positive
void fe_sub(const fe51 a, fe51 result, const fe51 b) {
    result[0] = a[0] - b[0] + (2 * p_01);

UNROLL
    for (u8 i = 1; i < FELEN; i++) { result[i] = a[i] - b[i] + (2 * P); }
}

void fe_mul(const fe51 a, fe51 result, const fe51 b) {
    pseudo_u128 mul = mul_u64(a[0], b[0]); // clang-format off

    pseudo_u128 sub_res1 = u128_add(mul_u64(a[2], b[1]), mul_u64(a[3], b[0]));
    pseudo_u128 sub_res2 = u128_add(mul_u64(a[3], b[1]), mul_u64(a[4], b[0]));

    pseudo_u128 res1 = u128_add(mul_u64(a[0], b[2]), mul_u64(a[1], b[1]));
    pseudo_u128 res2 = u128_add(mul_u64(a[0], b[3]), mul_u64(a[1], b[2]));
    pseudo_u128 res3 = u128_add(mul_u64(a[0], b[4]), mul_u64(a[1], b[3]));
    pseudo_u128 res4 = u128_add(mul_u64(a[2], b[2]), sub_res2);

    pseudo_u128 add1 = u128_add(mul_u64(a[0], b[1]), mul_u64(a[1], b[0]));
    pseudo_u128 add2 = u128_add(res1, mul_u64(a[2], b[0]));
    pseudo_u128 add3 = u128_add(res2, sub_res1);
    pseudo_u128 add4 = u128_add(res3, res4);

    // clang-format on
}

// NOLINTEND(readability-identifier-length)
