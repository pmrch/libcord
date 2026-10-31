#include "log.h"
#include "resolve.h"
#include "utils.h"

int main(void) {
    u8 num_decoded = 0;

    u32 addresses[DECODED_ANSWERS] = {0};
    get_addr_ipv4("discord.com", addresses, &num_decoded);

    u8 formatted_addr[BITS16] = {0};
    get_formatted_ipv4(addresses[0], formatted_addr);

    LOG_INFO("Got a whole lot of %s", formatted_addr);
    return 0;
}
