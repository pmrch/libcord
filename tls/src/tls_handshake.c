#include "types.h"

#include <errno.h>
#include <sys/socket.h>

int64_t tls_record_send(int32_t sock_fd, u8 type, const u8 *payload, u64 len);
int64_t tls_record_recv(int32_t sock_fd, u8 *type_out, u8 *buf, u64 bufsize);

int64_t tls_record_send(int32_t sock_fd, const u8 TYPE, const u8 *payload, u64 len) {
    u64 sent = 0;
    (void)TYPE;

    const u8 *ptr = payload;
    while (sent < len) {
        int64_t sss = send(sock_fd, ptr + sent, len - sent, 0);
        if (sss < 0 && errno == EINTR) { continue; }
        if (sss < 0) { return -1; }
        sent += (u64)(sss);
    }

    return (int64_t)sent;
}
