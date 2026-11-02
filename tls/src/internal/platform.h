#ifndef PLATFORM_H
#define PLATFORM_H

#ifdef _WIN32
#include "win32.h"
#else
#include "posix.h"
#endif

void   start_timer(Timer *timer);
void   end_timer(Timer *timer);
double get_timer_ns(Timer *timer);

static inline const char *get_error_string(int errnum) {
#if defined(_MSC_VER)
    static char buf[BITS64 * 2];
    (void)strerror_s(buf, sizeof(buf), errnum);
    return buf;
#else
    return strerror(errnum);
#endif
}

#endif
