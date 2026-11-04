#ifndef PLATFORM_H
#define PLATFORM_H

#include <string.h>

#ifdef _WIN32
#include "win32.h"
#else
#include "posix.h"
#endif

void   start_timer(Timer *timer);
void   end_timer(Timer *timer);
double get_timer_ns(Timer *timer);

#ifdef _MSC_VER
#pragma comment(lib, "ws2_32.lib")
#include <WinSock2.h>

#define sock_close(fd) do {\
    closesocket(sock_fd); \
    WSACleanup(); \
} while (0)

#define RSOCK_TYPE SOCK_DGRAM

#else
#define RSOCK_TYPE (SOCK_DGRAM | SOCK_CLOEXEC)
#define sock_close(fd) do {\
    close(sock_fd); \
} while (0)

#endif

static inline const char *get_error_string(int errnum) {
#ifdef _MSC_VER
    static char buf[BITS64 * 2];
    (void)strerror_s(buf, sizeof(buf), errnum);
    return buf;
#else
    return strerror(errnum);
#endif
}

#endif
