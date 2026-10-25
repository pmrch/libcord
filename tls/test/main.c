#include "resolve.h"
#include <stdio.h>

int main(void) {
    uint32_t *addr = get_addr_ipv4("google.com");
    printf("Got the following address: %p\n", (void *)addr);

    return 0;
}
