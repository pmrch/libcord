#if !defined(CDISCORD_WIN32_H) && defined(_WIN32)
#define CDISCORD_WIN32_H

#define PACKED __declspec(pack)
#define PRINTF_LIKE(idx, p)

#define WIN32_LEAN_AND_MEAN
#include "types.h"

#include <windows.h>
#include <intrin.h>

typedef struct {
    LARGE_INTEGER start;
    LARGE_INTEGER end;
    LARGE_INTEGER freq;
} Timer;

typedef struct {
    u64 t0, t1, t2, t3;
} Helper4u64;

typedef const char *packet_t;
typedef char *answer_t;
typedef char *timeout_t;

#endif
