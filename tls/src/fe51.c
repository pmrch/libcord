#include "fe51.h"
#include "crypt.h"
#include "log.h"
#include "utils.h"

#include <inttypes.h>
#include <string.h>

// NOLINTBEGIN(readability-identifier-length)
void fe_add(const fe51 a, fe51 result, const fe51 b) {
#pragma unroll
    for (u8 i = 0; i < FELEN; i++) { result[0] = a[0] + b[0]; }
}

// The reason you add 2 * P is to prevent underflow you’re essentially adding a multiple of p (which doesn’t change the value mod p)
// to ensure the result stays positive
void fe_sub(const fe51 a, fe51 result, const fe51 b) {
    result[0] = a[0] - b[0] + (2 * p_01);

#pragma unroll
    for (u8 i = 1; i < FELEN; i++) { result[i] = a[i] - b[i] + (2 * P); }
}

void fe_mul(const fe51 a, fe51 result, const fe51 b) {
    pseudo_u128 sub_res1 = u128_add(mul_u64(a[2], b[1]), mul_u64(a[3], b[0]));
    pseudo_u128 sub_res2 = u128_add(mul_u64(a[3], b[1]), mul_u64(a[4], b[0]));

    pseudo_u128 res1 = u128_add(mul_u64(a[0], b[2]), mul_u64(a[1], b[1]));
    pseudo_u128 res2 = u128_add(mul_u64(a[0], b[3]), mul_u64(a[1], b[2]));
    pseudo_u128 res3 = u128_add(mul_u64(a[0], b[4]), mul_u64(a[1], b[3]));
    pseudo_u128 res4 = u128_add(mul_u64(a[2], b[2]), sub_res2);

    pseudo_u128 add0 = mul_u64(a[0], b[0]);
    pseudo_u128 add1 = u128_add(mul_u64(a[0], b[1]), mul_u64(a[1], b[0]));
    pseudo_u128 add2 = u128_add(res1, mul_u64(a[2], b[0]));
    pseudo_u128 add3 = u128_add(res2, sub_res1);
    pseudo_u128 add4 = u128_add(res3, res4);

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

    pseudo_u128 conv = {0};

    result[0] = add0.low_bits & MASK51;
    conv      = u128_shr51(add0);
    add1      = u128_add(add1, conv);

    result[1] = add1.low_bits & MASK51;
    conv      = u128_shr51(add1);
    add2      = u128_add(add2, conv);

    result[2] = add2.low_bits & MASK51;
    conv      = u128_shr51(add2);
    add3      = u128_add(add3, conv);

    result[3] = add3.low_bits & MASK51;
    conv      = u128_shr51(add3);
    add4      = u128_add(add4, conv);

    result[4] = add4.low_bits & MASK51;
    conv      = u128_shr51(add4);

    pseudo_u128 c19 = mul_u64(conv.low_bits, PRIME);
    result[0] += c19.low_bits;
}

void fe_sq(fe51 out, const fe51 a) { fe_mul(a, out, a); }

void fe_pow2k(fe51 out, const fe51 input, u64 k) {
    fe_sq(out, input);

#pragma unroll 4
    for (u64 i = 0; i < k; i++) { fe_sq(out, out); }
}

// NOLINTBEGIN(readability-magic-numbers)
void fe_pow22501(fe51 z_2, fe51 *out) {
    fe51 terms[20] = {0};

    // let t0  = self.square();
    memcpy(terms[0], z_2, sizeof(terms[0]));
    fe_sq(terms[0], terms[0]);

    // let t1  = t0.square().square();
    memcpy(terms[1], terms[0], sizeof(terms[0]));
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

    memcpy(out[0], terms[19], sizeof(terms[19]));
    memcpy(out[1], terms[3], sizeof(terms[3]));
}
// NOLINTEND(readability-magic-numbers)

/*
 fn pow22501(&self) -> (FieldElement, FieldElement) {
        //
        // Each temporary variable t_i is of the form (self)^e_i.
        // Squaring t_i corresponds to multiplying e_i by 2,
        // so the pow2k function shifts e_i left by k places.
        // Multiplying t_i and t_j corresponds to adding e_i + e_j.
        //
        // Temporary t_i                      Nonzero bits of e_i
        //
        let t0  = self.square();           // 1         e_0 = 2^1
        let t1  = t0.square().square();    // 3         e_1 = 2^3
        let t2  = self * &t1;              // 3,0       e_2 = 2^3 + 2^0
        let t3  = &t0 * &t2;               // 3,1,0
        let t4  = t3.square();             // 4,2,1
        let t5  = &t2 * &t4;               // 4,3,2,1,0
        let t6  = t5.pow2k(5);             // 9,8,7,6,5
        let t7  = &t6 * &t5;               // 9,8,7,6,5,4,3,2,1,0
        let t8  = t7.pow2k(10);            // 19..10
        let t9  = &t8 * &t7;               // 19..0
        let t10 = t9.pow2k(20);            // 39..20
        let t11 = &t10 * &t9;              // 39..0
        let t12 = t11.pow2k(10);           // 49..10
        let t13 = &t12 * &t7;              // 49..0
        let t14 = t13.pow2k(50);           // 99..50
        let t15 = &t14 * &t13;             // 99..0
        let t16 = t15.pow2k(100);          // 199..100
        let t17 = &t16 * &t15;             // 199..0
        let t18 = t17.pow2k(50);           // 249..50
        let t19 = &t18 * &t13;             // 249..0

        (t19, t3)
    }
*/

// NOLINTEND(readability-identifier-length)
