#include "http.h"
#include "log.h"
#include "resolve.h"
#include "utils.h"

#define HTTPS_PORT 443

void connect_to_server(const char *hostname) {
    u32 addresses[DECODED_ANSWERS] = {0};
    u8  num_decoded;

    get_addr_ipv4(hostname, addresses, &num_decoded);
    u32 main_addr = addresses[0];

    saddr_in  addr = {.sin_addr.s_addr = htonl(main_addr), .sin_port = htons(HTTPS_PORT), .sin_family = AF_INET};
    socklen_t len  = sizeof(addr);

    int32_t sock_fd = socket(AF_INET, SOCK_CLOEXEC | SOCK_STREAM, IPPROTO_TCP);
    if (sock_fd < 0) {
        LOG_ERROR("Failed to open socket to %s", hostname);
        return;
    }

    if (connect(sock_fd, (saddr *)&addr, len) < 0) {
        char ipv[BITS16] = {0};
        get_formatted_ipv4(main_addr, (u8 *)ipv);

        LOG_ERROR("Failed to connect to server: %s", ipv);
        return;
    }

    
}
