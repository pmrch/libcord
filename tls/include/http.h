#ifndef HTTP_H
#define HTTP_H

#include "types.h"

void connect_to_server(const char *hostname, const u8 *client_hello, u16 client_hello_len, u8 *restrict my_private_key);

#endif
