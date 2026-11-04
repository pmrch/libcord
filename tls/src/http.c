#include "http.h"
#include "constants.h"
#include "log.h"
#include "platform.h"
#include "resolve.h"
#include "tls.h"
#include "utils.h"

#define HTTPS_PORT 443
#define PAYLOAD_SIZE 512

void connect_to_server(const char *hostname, const u8 *client_hello, u16 client_hello_len, u8 *restrict my_private_key) {
    u32 addresses[DECODED_ANSWERS] = {0};
    u8  num_decoded;

    LOG_DEBUG("Got hostname: %s", hostname);
    get_addr_ipv4(hostname, addresses, &num_decoded);
    u32 main_addr = addresses[0];

    saddr_in  addr = {.sin_addr.s_addr = htonl(main_addr), .sin_port = htons(HTTPS_PORT), .sin_family = AF_INET};
    socklen_t len  = sizeof(addr);

    sock_type_t sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (SOCK_CREATE_FAIL(sock_fd)) {
        LOG_ERROR("Failed to open socket to %s | %d", hostname, WSAGetLastError());
        return;
    }

    if (SOCK_CONN_FAIL(connect(sock_fd, (saddr *)&addr, len))) {
        char ipv[BITS16] = {0};
        get_formatted_ipv4(main_addr, (u8 *)ipv);

        LOG_ERROR("Failed to connect to server: %s, %d, %li", ipv, WSAGetLastError(), sock_fd);
        return;
    }

    u8 received[1 << (TEN + 1) << 2] = {0};
    tls_record_send(sock_fd, TYPE_HANDSHAKE, client_hello, client_hello_len);

    int64_t recvd      = 0;
    u64     bytes_read = 0;

    while (bytes_read < 90) {
        int64_t rec = recv(sock_fd, received + bytes_read, 128, 0);
        if (rec > 0) { bytes_read += rec; }
    }

    if (recvd < 0) {
        LOG_ERROR("%s", "Failed to receive answer from host!");
        return;
    }

    printf("\x1b[33mGot: ");
    for (u8 i = 0; i < SEVEN; i++) { printf("%02x ", received[i]); }
    printf("\x1b[0m\n");
}
