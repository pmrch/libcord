#ifndef TYPES_H
#define TYPES_H

#include "constants.h"
#include <stdint.h>

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t  u8;

typedef int64_t i64;
typedef int32_t i32;
typedef int16_t i16;
typedef int8_t  i8;

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

// NOLINTBEGIN (-Wunused-macros)
#define TRUE 1
#define FALSE (!TRUE)

#ifndef __SIZEOF_INT128__
typedef pseudo_u128 u128;
#else
typedef __uint128_t u128;
#endif

#ifdef _MSC_VER
#pragma comment(lib, "ws2_32.lib")
#include <WinSock2.h>

typedef SOCKET sock_type_t;
typedef int    socklen_t;

#define RSOCK_TYPE SOCK_DGRAM
#define RSOCK_STREAM_TYPE SOCK_STREAM

#else
#include <sys/socket.h>

#define RSOCK_TYPE (SOCK_DGRAM | SOCK_CLOEXEC)
#define RSOCK_STREAM_TYPE (SOCK_STREAM | SOCK_CLOEXEC)
typedef int32_t sock_type_t;

// NOLINTEND (-Wunused-macros)
#endif

#endif
