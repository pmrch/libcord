#if !defined(CDISCORD_WIN32_H) && defined(_WIN32)
#define CDISCORD_WIN32_H

#define PACKED __declspec(pack)
#define PRINTF_LIKE(idx, p)

#define GET_ERROR_CODE (WSAGetLastError())

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

#endif
