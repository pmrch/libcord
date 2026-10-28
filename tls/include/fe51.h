#ifndef FE51_H
#define FE51_H

#include "utils.h"

#define LIMBS 5

typedef u64 fe51[LIMBS];

// NOLINTBEGIN(readability-identifier-length)
void fe_add(const fe51 a, fe51 result, const fe51 b);
void fe_sub(const fe51 a, fe51 result, const fe51 b);
void fe_mul(const fe51 a, fe51 result, const fe51 b);
// NOLINTEND(readability-identifier-length)

#endif
