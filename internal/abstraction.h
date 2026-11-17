#ifndef ABSTRACTION_H
#define ABSTRACTION_H

// Utility macros
#define MIN(x, y) (x < y ? x : y)
#define MAX(x, y) (x > y ? x : y)

// Mass object free
#define TO_DFREE(obj) ((Freeable){free, obj, NULL, #obj})

#ifdef __clang__
#define TO_FREE(obj, destructor) ((Freeable){(Destructor)((void*)(destructor)), obj, #obj})
#else
#define TO_FREE(obj, destructor) ((Freeable){(Destructor)(destructor), obj, #obj})
#endif

#define FREE_ALL(...) \
    free_all( \
        (Freeable[]){ __VA_ARGS__ }, \
        (sizeof(((Freeable[]){ __VA_ARGS__ })) / sizeof(Freeable))\
    )

#define RET_ON_ERR(condition, retval, fmt, ...) \
    do { \
        if (condition) {\
            LOG_ERROR(fmt, __VA_ARGS__); \
            return retval; \
        } \
    } while (0)

#endif
