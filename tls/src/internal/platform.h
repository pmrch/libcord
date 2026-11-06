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
} while (0)

#define RSOCK_TYPE SOCK_DGRAM

#define SOCK_RECVFROM(s, buf, len, flags, from, fromlen) \
    recvfrom((s), (char*)(buf), (int)(len), (flags), (from), (int*)(fromlen))

#define SOCK_RECV(sock_fd, buf, len, flags) \
    recv((sock_fd), (char *)(buf), (i32)(len), (flags))

#define SOCKOPT_TIMEOUT_RECV(sock_fd, secs, failed) do {\
    DWORD timeout_ms = (secs) * 1000; \
    i64 res = (i64)(setsockopt((sock_fd), SOL_SOCKET, SO_RCVTIMEO, (const char *)&(timeout_ms), sizeof(timeout_ms))); \
    (failed) = (u8)(res != SOCKET_ERROR); \
} while (0)

#define SOCK_CREATE_FAIL(sock_fd) ((sock_fd) == INVALID_SOCKET)
#define SOCK_RECV_FAIL(recv_status) ((recv_status) == SOCKET_ERROR)

#define INIT_SOCK() do { \
    WSADATA wsaData; \
    WSAStartup(MAKEWORD(2, 2), &wsaData); \
} while (0)

#define SOCK_CONN_FAIL(retcode) ((retcode) == SOCKET_ERROR)
#define DESTROY_SOCK_FRAME() WSACleanup()

#else
// You'll also need to catch WSAEMSGSIZE and return (len) instead of -1
#define SOCK_RECVFROM(s, buf, len, flags, from, fromlen) \
    recvfrom((s), (buf), (len), (flags), (from), (fromlen))

#define SOCK_RECV(sock_fd, buf, len, flags) \
    recv((sock_fd), (u8 *)(buf), (u16)(len), (flags))

#define RSOCK_TYPE (SOCK_DGRAM | SOCK_CLOEXEC)
#define sock_close(fd) do {\
    close(sock_fd); \
} while (0)

#define SOCKOPT_TIMEOUT_RECV(sock_fd, secs, failed) do {\
    struct timeval tv = {0}; \
    tv.tv_sec = (secs); \
    i64 res = (i64)setsockopt(sock_fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)); \
    (failed) = (u8)(res >= 0); \
} while (0)

#define SOCK_CREATE_FAIL(sock_fd) ((sock_fd) == -1)
#define SOCK_RECV_FAIL(recv_status) ((recv_status) < 0)
#define SOCK_CONN_FAIL(retcode) ((retcode) < 0)

#define INIT_SOCK()
#define DESTROY_SOCK_FRAME()

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
