#if !defined(CDISCORD_POSIX_H) && !defined(_WIN32)
#define CDISCORD_POSIX_H

#include "constants.h"
#include "types.h"

#include <errno.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

// NOLINTBEGIN (-Wunused-macros)

#define PACKED __attribute__((packed))
#define PRINTF_LIKE(idx, p) __attribute__((format(printf, idx, p)))

#define GET_ERROR_CODE (errno)

typedef struct timespec tspec;

typedef struct {
    tspec   start;
    tspec   end;
    double *extra;
} Timer;

typedef const u8 *packet_t;
typedef u8       *answer_t;

static inline double get_clk_ns(tspec *start, tspec *end) {
    return ((double)(end->tv_sec - start->tv_sec) * ONE_BILLION) + (double)(end->tv_nsec - start->tv_nsec);
}

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

// NOLINTEND (-Wunused-macros)

#endif
