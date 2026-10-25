#ifndef UTILS_H
#define UTILS_H

#include "platform.h"
#include <stdint.h>

#define TO_DFREE(obj) ((Freeable){free, obj, NULL, #obj})
#define TO_FREE(obj, destructor) ((Freeable){(Destructor)((void*)destructor), obj, NULL, #obj})

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

void free_all(Freeable *objs, u64 num);
u16  generate_random_u16(void);

#endif
