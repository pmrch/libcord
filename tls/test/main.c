#include "crypt.h"
#include "log.h"
#include "utils.h"

#include <time.h>

#define ADDR "discord.com"
#define N 2

static void test_fe(void) {
    const char *hex_scalar   = "4b66e9d4d1b4673c5ad22691957d6af5c11b6421e0ea01d42ca4169e7918ba0d";
    const char *hex_ucoord   = "e5210f12786811d3f4b7959d0538ae2c31dbe7106fc03c3efc4cd549c715a493";
    const char *hex_expected = "95cbde9476e8907d7aade45cb4b873f88b595a68799fa152e6f8f7647aac7957";

    u8 bytes_scalar[BYTES];
    u8 bytes_ucoord[BYTES];
    u8 expected[BYTES];

    u8   output[BYTES] = {0};
    fe51 field_output  = {0};

    hex_to_bytes(hex_scalar, bytes_scalar);
    hex_to_bytes(hex_ucoord, bytes_ucoord);
    hex_to_bytes(hex_expected, expected);

    struct timespec start = {0};
    struct timespec end   = {0};

    // NOLINTNEXTLINE
    for (u64 i = 0; i < 10; i++) { curve25519(bytes_scalar, bytes_ucoord, field_output); }

    clock_gettime(CLOCK_MONOTONIC, &start); // NOLINTNEXTLINE
    for (u64 i = 0; i < N; i++) {
        curve25519(bytes_scalar, bytes_ucoord, field_output);
        /*encode_u_coordinate(output, field_output);

        if (memcmp(output, expected, BYTES) == 0) { continue; }

        printf("X25519 test FAILED!\n");
        printf("Expected: ");
        for (u8 j = 0; j < BYTES; j++) { printf("%02x", expected[j]); }

        printf("\nActual:   ");
        for (u8 k = 0; k < BYTES; k++) { printf("%02x", output[k]); }

        printf("\n");*/
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    printf("%.2f us/op\n", (double)(get_clk_ns(&start, &end)) / (double)N / 1000.0);
}

int main(void) {
    /*u8 first_ip[4] = {0};

    uint32_t *addr = get_addr_ipv4(ADDR);
    RET_ON_ERR(addr == NULL, -1, "Failed to get IP addresses for %s", ADDR);
    ipv4_word_to_bytes(addr[0], first_ip);*/

    test_fe();

    // free(addr);
    return 0;
}
