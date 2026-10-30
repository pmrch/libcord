#include "utils.h"

#ifndef HAS_UINT128
#include "crypt.h"
#include "fe51_no_u128.h"

typedef struct {
    u64 p0, p1, p2, p3;
} Products;

// NOLINTBEGIN(readability-identifier-length)
void fe_mul(const u64 *restrict a, u64 *restrict result, const u64 *restrict b) {
    LimbRepr rights;
    LimbRepr lefts;

    memcpy(&rights, a, sizeof(rights));
    memcpy(&lefts, b, sizeof(lefts));
    pseudo_u128 bits = {0};

    pseudo_u128 term0 = {0};
    pseudo_u128 term1 = {0};
    pseudo_u128 term2 = {0};
    pseudo_u128 term3 = {0};
    pseudo_u128 term4 = {0};

    // t0
    mul64(rights.t0, &term0, lefts.t0);

    // t1
    mul64(rights.t0, &term1, lefts.t1);
    // mul64(rights.t1, &term1, u64 term2)
}
// NOLINTEND(readability-identifier-length)

#endif
