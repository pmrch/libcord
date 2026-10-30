#ifndef UTILS_H
#define UTILS_H

#include "platform.h"

#include <stdint.h>
#include <string.h>

// Utility macros
#define MIN(x, y) (x < y ? x : y)
#define MAX(x, y) (x > y ? x : y)

#define MASK32 0xFFFFFFFFULL

#define ONE_BILLION 1000000000ULL
#define ONE_MILLION 1000000ULL

// These two make up the 64 bits
#define FULL_BITS 64ULL
#define LIMB_SIZE 51ULL
#define LIMB_STEP 13ULL
#define LIMBS 5ULL

#define OCT1 24
#define OCT2 16
#define OCT3 8

#define IPV4BUF (u64)16
#define IPV4 (u8)0xFF

#define ONE_BYTE 8ULL
#define BITS32 32
#define BITS64 64

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

#if defined(__SIZEOF_INT128__)
typedef __uint128_t u128;
#define HAS_UINT128

#else
typedef struct pseudo_u128 u128;

#endif

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t  u8;

typedef void (*Destructor)(void *);
typedef u64 fe51[LIMBS];

typedef struct {
    Destructor  func;
    void       *obj;
    const void *addr;
    const char *name;
} Freeable;

typedef struct {
    u64 low_bits;
    u64 high_bits;
} pseudo_u128;

typedef struct {
    u64 t0, t1, t2, t3, t4;
} LimbRepr;

typedef struct {
    u64 t0, t1, t2, t3;
} Helper4u64;

unsigned char *get_formatted_ipv4(u32 word);

void u64_to_bytes(u64 value, u8 out[ONE_BYTE]);
void ipv4_word_to_bytes(u32 word, u8 bytes[4]);
void free_all(Freeable *objs, u64 num);

u16 generate_random_u16(void);
u64 u64_from_bytes_le(const u8 bytes[ONE_BYTE]);

#ifndef HAS_UINT128
u128 mul_u64(u64 left, u64 right);
u128 u128_add(u128 left, u128 right);
u128 u128_add_u64(u128 num_a, u64 num_b);

u128 u128_shr51(u128 num_a);

#else
static inline u128 mul_u64(u64 term1, u64 term2) { return (u128)term1 * (u128)term2; }
static inline u128 u128_add(u128 term1, u128 term2) { return term1 + term2; }
static inline u128 u128_add_u64(u128 term1, u64 term2) { return term1 + term2; }

static inline u64 u128_shr51(u128 term1) { return (u64)(term1 >> LIMB_SIZE); }

#endif

// NOLINTBEGIN(readability-identifier-length, readability-magic-numbers)
void hex_to_bytes_le(const char *hex_str, uint8_t bytes[32]);
void hex_to_bytes(const char *hex, u8 *bytes);
void bytes_to_fe51(const uint8_t bytes[32], u64 f[5]);
// NOLINTEND(readability-identifier-length, readability-magic-numbers)

static inline u64 get_clk_ns(struct timespec *start, struct timespec *end) {
    return ((u64)(end->tv_sec - start->tv_sec) * ONE_BILLION) + (u64)(end->tv_nsec - start->tv_nsec);
}

#endif
