#ifndef TYPES_H
#define TYPES_H

#include "constants.h"
#include <stdint.h>

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t  u8;

typedef void (*Destructor)(void *);
typedef u64 fe51[LIMBS];

typedef struct {
    Destructor  func;
    void       *obj;
    const char *name;
} Freeable;

typedef struct {
    u64 low_bits;
    u64 high_bits;
} pseudo_u128;

typedef struct {
    u64 t0, t1, t2, t3, t4;
} LimbRepr;

#if !defined(__SIZEOF_INT128__)
typedef pseudo_u128 u128;
#else
typedef __uint128_t u128;
#endif

#endif
