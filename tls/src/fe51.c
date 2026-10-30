#include "fe51.h"
#include "crypt.h"
#include "log.h"
#include "utils.h"

#include <string.h>

#define CROSS_P 2ULL
#define CROSS_P38 38ULL
#define CROSS_P19 19ULL

#define CARRY_NEXT(left, result, index, next, retain) do { \
    u64 carry##index = u128_low(u128_shr(left, LIMB_SIZE)); \
    (result)[(index)] = u128_low(left) & MASK51; \
    (next) += carry##index; \
    write_to_retain(retain, carry##index); \
} while (0)

static inline void write_to_retain(u64 *value, const u64 TO_WRITE) {
    if (value == NULL) { return; }
    *value = TO_WRITE;
}

// NOLINTBEGIN(readability-identifier-length, readability-magic-numbers)
void fe_frombytes(const u8 *restrict bytes, u64 *restrict output) {
    u64 terms[4] = {0};
    u8  slice[8] = {0};

    for (u8 i = 0; i < 4; i++) {
        memcpy(slice, bytes + (i * ONE_BYTE), sizeof(slice));
        u64 term = u64_from_bytes_le(slice);
        terms[i] = term;
    }

    pack_values(terms, output);
}

void fe_mul(const u64 *restrict a, u64 *restrict result, const u64 *restrict b) {
    u128 sub_res1 = u128_add(mul_u64(a[2], b[1]), mul_u64(a[3], b[0]));
    u128 sub_res2 = u128_add(mul_u64(a[3], b[1]), mul_u64(a[4], b[0]));

    u128 res1 = u128_add(mul_u64(a[0], b[2]), mul_u64(a[1], b[1]));
    u128 res2 = u128_add(mul_u64(a[0], b[3]), mul_u64(a[1], b[2]));
    u128 res3 = u128_add(mul_u64(a[0], b[4]), mul_u64(a[1], b[3]));
    u128 res4 = u128_add(mul_u64(a[2], b[2]), sub_res2);

    u128 add0 = mul_u64(a[0], b[0]); // sub_res2 can be discarded
    u128 add1 = u128_add(mul_u64(a[0], b[1]), mul_u64(a[1], b[0]));
    u128 add2 = u128_add(res1, mul_u64(a[2], b[0]));
    u128 add3 = u128_add(res2, sub_res1);
    u128 add4 = u128_add(res3, res4);

    // reduce t5-t8 by multiplying by 19
    add0 = u128_add(add0, mul_u64(a[1] * PRIME, b[4]));
    add0 = u128_add(add0, mul_u64(a[2] * PRIME, b[3]));
    add0 = u128_add(add0, mul_u64(a[3] * PRIME, b[2]));
    add0 = u128_add(add0, mul_u64(a[4] * PRIME, b[1]));

    add1 = u128_add(add1, mul_u64(a[2] * PRIME, b[4]));
    add1 = u128_add(add1, mul_u64(a[3] * PRIME, b[3]));
    add1 = u128_add(add1, mul_u64(a[4] * PRIME, b[2]));

    add2 = u128_add(add2, mul_u64(a[3] * PRIME, b[4]));
    add2 = u128_add(add2, mul_u64(a[4] * PRIME, b[3]));

    add3 = u128_add(add3, mul_u64(a[4] * PRIME, b[4]));

    // Map terms to radix-51 limbs
    result[0] = (u64)(add0)&MASK51;
    add1 += add0 >> LIMB_SIZE;

    result[1] = (u64)(add1)&MASK51;
    add2 += add1 >> LIMB_SIZE;

    result[2] = (u64)(add2)&MASK51;
    add3 += add2 >> LIMB_SIZE;

    result[3] = (u64)(add3)&MASK51;
    add4 += add3 >> LIMB_SIZE;

    result[4] = (u64)(add4)&MASK51;
    u64 carry = (u64)(add4 >> LIMB_SIZE);

    result[0] += (carry * PRIME);

    // THE FIX: We must propagate the carry from result[0] before we return!
    // Otherwise, result[0] might be > 2^51 when we exit.
    result[1] += result[0] >> LIMB_SIZE;
    result[0] &= MASK51;
}

void fe_sq(u64 *out, const u64 *a) { /*fe_mul(a, out, a);*/
    LimbRepr limbs = {a[0], a[1], a[2], a[3], a[4]};

    u128 m0 = u128_add(mul_u64(limbs.t0, limbs.t0), mul_u64(limbs.t1, limbs.t4 * CROSS_P38));
    u128 m1 = u128_add(mul_u64(limbs.t0, limbs.t1 * CROSS_P), mul_u64(limbs.t2, limbs.t4 * CROSS_P38));
    u128 m2 = u128_add(mul_u64(limbs.t0, limbs.t2 * CROSS_P), mul_u64(limbs.t1, limbs.t1));
    u128 m3 = u128_add(mul_u64(limbs.t0, limbs.t3 * CROSS_P), mul_u64(limbs.t1, limbs.t2 * CROSS_P));
    u128 m4 = u128_add(mul_u64(limbs.t0, limbs.t4 * CROSS_P), mul_u64(limbs.t1, limbs.t3 * CROSS_P));

    m0 = u128_add(m0, mul_u64(limbs.t2, limbs.t3 * CROSS_P38));
    m1 = u128_add(m1, mul_u64(limbs.t3, limbs.t3 * CROSS_P19));
    m2 = u128_add(m2, mul_u64(limbs.t3, limbs.t4 * CROSS_P38));
    m3 = u128_add(m3, mul_u64(limbs.t4, limbs.t4 * CROSS_P19));
    m4 = u128_add(m4, mul_u64(limbs.t2, limbs.t2));

    // MAP
    out[0] = (u64)(m0)&MASK51;
    m1 += m0 >> LIMB_SIZE;

    out[1] = (u64)(m1)&MASK51;
    m2 += m1 >> LIMB_SIZE;

    out[2] = (u64)(m2)&MASK51;
    m3 += m2 >> LIMB_SIZE;

    out[3] = (u64)(m3)&MASK51;
    m4 += m3 >> LIMB_SIZE;

    out[4]    = (u64)(m4)&MASK51;
    u64 carry = (u64)(m4 >> LIMB_SIZE);

    out[0] += (carry * PRIME);

    // THE FIX: We must propagate the carry from result[0] before we return!
    // Otherwise, result[0] might be > 2^51 when we exit.
    out[1] += out[0] >> LIMB_SIZE;
    out[0] &= MASK51;
}

static inline void fe_pow2k(fe51 out, const fe51 input, u64 k) {
    memcpy(out, input, sizeof(fe51));
    for (u64 i = 0; i < k; i++) { fe_sq(out, out); }
}

void fe_pow22501(const fe51 z_2, fe51 *out) {
    fe51 terms[20] = {0};

    // let t0  = self.square();
    fe_sq(terms[0], z_2);

    // let t1  = t0.square().square();
    fe_sq(terms[1], terms[0]);
    fe_sq(terms[1], terms[1]);

    fe_mul(z_2, terms[2], terms[1]);      // let t2  = self * &t1;
    fe_mul(terms[0], terms[3], terms[2]); // let t3  = &t0 * &t2;
    fe_sq(terms[4], terms[3]);            // let t4  = t3.square();

    fe_mul(terms[2], terms[5], terms[4]);    // let t5  = &t2 * &t4;
    fe_pow2k(terms[6], terms[5], 5);         // let t6  = t5.pow2k(5);
    fe_mul(terms[6], terms[7], terms[5]);    // let t7  = &t6 * &t5;
    fe_pow2k(terms[8], terms[7], 10);        // let t8 = t7.pow2k(10);
    fe_mul(terms[8], terms[9], terms[7]);    // let t9  = &t8 * &t7;
    fe_pow2k(terms[10], terms[9], 20);       // let t10 = t9.pow2k(20);
    fe_mul(terms[10], terms[11], terms[9]);  // let t11 = &t10 * &t9;
    fe_pow2k(terms[12], terms[11], 10);      // let t12 = t11.pow2k(10);
    fe_mul(terms[12], terms[13], terms[7]);  // let t13 = &t12 * &t7;
    fe_pow2k(terms[14], terms[13], 50);      // let t14 = t13.pow2k(50);
    fe_mul(terms[14], terms[15], terms[13]); // let t15 = &t14 * &t13;
    fe_pow2k(terms[16], terms[15], 100);     // let t16 = t15.pow2k(100);
    fe_mul(terms[16], terms[17], terms[15]); // let t17 = &t16 * &t15;
    fe_pow2k(terms[18], terms[17], 50);      // let t18 = t17.pow2k(50);
    fe_mul(terms[18], terms[19], terms[13]); // let t19 = &t18 * &t13;

    // (t19, t3)
    memcpy(out[0], terms[19], sizeof(terms[19]));
    memcpy(out[1], terms[3], sizeof(terms[3]));
}

void fe_invert(fe51 out, const fe51 input) {
    fe51 terms[3] = {0};

    fe_pow22501(input, terms);
    fe_pow2k(terms[2], terms[0], 5);
    fe_mul(terms[2], out, terms[1]);
}
// NOLINTEND(readability-identifier-length, readability-magic-numbers)
