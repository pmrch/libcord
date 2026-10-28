#ifndef FE51_H
#define FE51_H

#include "utils.h"

#define LIMBS 5

typedef u64 fe51[LIMBS];

void fe_add(const fe51 left, fe51 result, const fe51 right);
void fe_sub(const fe51 left, fe51 result, const fe51 right);

#endif
