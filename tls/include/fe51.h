#ifndef FE51_H
#define FE51_H

#include "utils.h"

#define LIMBS 5ULL
#define FELEN 5ULL
#define PRIME 19ULL

#define p_01 0x7FFFFFFFFFFEDULL
#define P 0x7FFFFFFFFFFFFULL

typedef u64 fe51[LIMBS];

// NOLINTBEGIN(readability-identifier-length)
void fe_add(const fe51 a, fe51 result, const fe51 b);
void fe_sub(const fe51 a, fe51 result, const fe51 b);
void fe_mul(const fe51 a, fe51 result, const fe51 b);

void fe_pow2k(fe51 out, const fe51 input, u64 k);
void fe_sq(fe51 out, const fe51 a);
// NOLINTEND(readability-identifier-length)

#endif
