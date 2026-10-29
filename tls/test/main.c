#include "crypt.h"
#include "http.h"
#include "log.h"
#include "resolve.h"
#include "utils.h"

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define ADDR "discord.com"
#define ONE_BILLION 1000000000ULL

/*static void try_crypt() {
    u8 output[BYTES] = {0};
    u8 bytes[BYTES]  = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x10,
                        0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20};

    fe51 felem = {0, 0, 0, 0, 0};
    decode_u_coordinate(felem, bytes);
    encode_u_coordinate(output, felem);

    printf("input: ");

#pragma unroll
    for (u8 i = 0; i < BYTES; i++) { printf("%#X", bytes[i]); }
    printf("\nOutput: ");

#pragma unroll
    for (u8 i = 0; i < BYTES; i++) { printf("%#X", output[i]); }
    printf("\n");
}*/

int main(void) {
    u8 first_ip[4] = {0};

    uint32_t *addr = get_addr_ipv4(ADDR);
    RET_ON_ERR(addr == NULL, -1, "Failed to get IP addresses for %s", ADDR);

    LOG_INFO("Got the following address: %p\n", (void *)addr);
    ipv4_word_to_bytes(addr[0], first_ip);

    // connect_to_server(ADDR);

    struct timespec start = {0};
    struct timespec end   = {0};
    clock_gettime(CLOCK_MONOTONIC, &start);
    clock_gettime(CLOCK_MONOTONIC, &end);
    /*#pragma unroll
        for (u8 i = 0; i < FELEN; i++) { printf("%lu ", r[i]); }

        double part = ((double)(end.tv_sec - start.tv_sec) * ONE_BILLION) + (double)(end.tv_nsec - start.tv_nsec);
        printf("\nTook %.02f us\n", part / 1000.0);

        u64 test = 11258999068426252 * 19;
        LOG_INFO("Shit: 0x%016" PRIX64 "\n", (11258999068426252 * 19));

        pseudo_u128 tes2t = mul_u64(2251799813685247, 2251799813685247);
        printf("test.lo: %lu\n", tes2t.low_bits);
        printf("test.hi: %lu\n", tes2t.high_bits);
    */
    free(addr);
    return 0;
}
