#include "crypt.h"
#include "utils.h"

#define N 1000

static void test_fe(void) {
    const char *hex_scalar = "4b66e9d4d1b4673c5ad22691957d6af5c11b6421e0ea01d42ca4169e7918ba0d";
    const char *hex_ucoord = "e5210f12786811d3f4b7959d0538ae2c31dbe7106fc03c3efc4cd549c715a493";
    // (Expected output is just for the sanity check, not used in the loop)

    u8 bytes_scalar[BITS32] = {0};
    u8 bytes_ucoord[BITS32] = {0};

    // u8   output[BITS32] = {0};
    fe51 field_output = {0};

    hex_to_bytes(hex_scalar, bytes_scalar);
    hex_to_bytes(hex_ucoord, bytes_ucoord);

    // Warmup (prevents page faults and cold caches from skewing the first batch)
    for (u64 i = 0; i < BITS16; i++) { curve25519(bytes_scalar, bytes_ucoord, field_output); }

    Timer timer = {0};
    start_timer(&timer);
    for (u64 i = 0; i < N; i++) { curve25519(bytes_scalar, bytes_ucoord, field_output); }
    end_timer(&timer);

    // Print the result in microseconds per operation
    printf("%.2f us/op\n", (get_timer_ns(&timer)) / (double)N / 1000.0);
}

int main(void) {
    /*u8 num_decoded = 0;

    u32 addresses[DECODED_ANSWERS] = {0};
    get_addr_ipv4("discord.com", addresses, &num_decoded);

    u8 formatted_addr[BITS16] = {0};
    get_formatted_ipv4(addresses[0], formatted_addr);

    LOG_INFO("Got a whole lot of %s", formatted_addr);*/
    test_fe();
    return 0;
}
