#if !defined(CDISCORD_POSIX_H) && !defined(_WIN32)
#define CDISCORD_POSIX_H

#include "constants.h"
#include <sys/time.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>

#define PACKED __attribute__((packed))
#define PRINTF_LIKE(idx, p) __attribute__((format(printf, idx, p)))

typedef struct timespec tspec;

typedef struct {
    tspec   start;
    tspec   end;
    double *extra;
} Timer;

static inline double get_clk_ns(tspec *start, tspec *end) {
    return ((double)(end->tv_sec - start->tv_sec) * ONE_BILLION) + (double)(end->tv_nsec - start->tv_nsec);
}

#endif
