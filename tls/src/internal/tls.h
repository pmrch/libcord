#ifndef CDISCORD_TLS_H
#define CDISCORD_TLS_H

#include "types.h"

u16 build_client_hello(u8 *restrict buffer, const char *hostname, u16 hname_len, const u8 *restrict pubkey);

i64 tls_record_recv(sock_type_t sock_fd, u8 *restrict buf, u16 bufsize);
i64 tls_record_send(sock_type_t sock_fd, u8 type, const u8 *payload, u16 len);

#endif
