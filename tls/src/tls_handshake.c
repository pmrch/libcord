#include "platform.h"
#include "resolve.h"
#include "types.h"

#include <errno.h>

int64_t tls_record_send(sock_type_t sock_fd, u8 type, const u8 *payload, socklen_t len);
int64_t tls_record_recv(sock_type_t sock_fd, u8 *type_out, u8 *buf, socklen_t bufsize);

int64_t tls_record_send(sock_type_t sock_fd, const u8 TYPE, const u8 *payload, socklen_t len) {
    socklen_t sent = 0;
    (void)TYPE;

    packet_t ptr = (packet_t)payload;
    while (sent < len) {
        int64_t sss = send(sock_fd, ptr + sent, len - sent, 0);
        if (sss < 0 && errno == EINTR) { continue; }
        if (sss < 0) { return -1; }
        sent += (socklen_t)sss;
    }

    return (int64_t)sent;
}
