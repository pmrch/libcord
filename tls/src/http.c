#include <stdlib.h>

#include "log.h"
#include "resolve.h"
#include "utils.h"

#define HTTPS_PORT 443

void connect_to_server(const char *hostname) {
    u32 *addresses = get_addr_ipv4(hostname);
    if (addresses == NULL) { return; }

    u32 main_addr = addresses[0];

    saddr_in  addr = {.sin_addr.s_addr = htonl(main_addr), .sin_port = htons(HTTPS_PORT), .sin_family = AF_INET};
    socklen_t len  = sizeof(addr);

    int32_t sock_fd = socket(AF_INET, SOCK_CLOEXEC | SOCK_STREAM, IPPROTO_TCP);
    if (sock_fd < 0) {
        LOG_ERROR("Failed to open socket to %s", hostname);
        free(addresses);
        return;
    }

    free(addresses);
}
