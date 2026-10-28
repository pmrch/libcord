#ifndef UTILS_H
#define UTILS_H

#include "platform.h"
#include <stdint.h>

// Utility macros
#define MIN(x, y) (x < y ? x : y)
#define MAX(x, y) (x > y ? x : y)

// These two make up the 64 bits
#define FULL_BITS (u64)64
#define LIMB_SIZE (u64)51
#define LIMB_STEP (u64)13

#define OCT1 24
#define OCT2 16
#define OCT3 8

#define IPV4BUF (u64)16
#define IPV4 (u8)0xFF

#define ONE_BYTE (u8)8

// Mass object free
#define TO_DFREE(obj) ((Freeable){free, obj, NULL, #obj})

#ifdef __clang__
#define TO_FREE(obj, destructor) ((Freeable){(Destructor)((void*)destructor), obj, NULL, #obj})
#else
#define TO_FREE(obj, destructor) ((Freeable){(Destructor)(destructor), obj, NULL, #obj})
#endif

#define FREE_ALL(...) (\
    free_all( \
        (Freeable[]){ __VA_ARGS__ }, \
        (sizeof(((Freeable[]){ __VA_ARGS__ })) / sizeof(Freeable))\
    ) \
)

// NOLINTBEGIN(altera-unroll-loops)
#define RET_ON_ERR(condition, retval, fmt, ...) \
    do { \
        if (condition) {\
            LOG_ERROR(fmt, __VA_ARGS__); \
            return retval; \
        } \
    } while (0)
// NOLINTEND(altera-unroll-loops)

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t  u8;

typedef void (*Destructor)(void *);

typedef struct ALIGN(32) {
    Destructor  func;
    void       *obj;
    const void *addr;
    const char *name;
} Freeable;

typedef struct ALIGN(16) {
    u64 low_bits;
    u64 high_bits;
} pseudo_u128;

unsigned char *get_formatted_ipv4(u32 word);

void u64_to_bytes(u64 value, u8 out[ONE_BYTE]);
void ipv4_word_to_bytes(u32 word, u8 bytes[4]);
void free_all(Freeable *objs, u64 num);

u16 generate_random_u16(void);
u64 u64_from_bytes_le(const u8 bytes[ONE_BYTE]);

pseudo_u128 mul_u64(u64 left, u64 right);
pseudo_u128 u128_add(pseudo_u128 left, pseudo_u128 right);
pseudo_u128 u128_add_u64(pseudo_u128 num_a, u64 num_b);

u64 u128_shr51(pseudo_u128 num_a);

#endif
