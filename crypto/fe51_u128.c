#include "types.h"

typedef struct {
    u64 p0, p1, p2, p3;
} Products;

#if !defined(__SIZEOF_INT128__)
#include "crypt.h"
#include "fe51_no_u128.h"
#include "utils.h"

// NOLINTEND(readability-identifier-length)

#endif
