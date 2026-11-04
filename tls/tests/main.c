#include "constants.h"
#include "crypt.h"
#include "http.h"
#include "log.h"
#include "tls.h"
#include "utils.h"

#include <string.h>

// #define BYTES 32
#define MAX_DOMAIN_LEN 254
#define ADDR "discord.com"
#define HELLO_SIZE 512
// #define N 100

int main(void) {
    INIT_SOCK();

    u8 private_key[BITS32] = {0};
    u8 public_key[BITS32]  = {0};
    u8 basepoint[BITS32]   = {NINE};

    fe51 output_field = {0};

    generate_random_bytes(BITS32, private_key);
    curve25519(private_key, basepoint, output_field);

    encode_u_coordinate(public_key, output_field);

    u8  hello_buf[HELLO_SIZE] = {0};
    u16 client_hello_len      = build_client_hello(hello_buf, ADDR, sizeof(ADDR) - 1, public_key);

    connect_to_server(ADDR, hello_buf, client_hello_len, private_key);
    // for (u16 i = 0; i < client_hello_len; i++) { printf("%02x ", hello_buf[i]); }
    printf("\n");
    // test_fe();
    //  free(addr);

    return 0;
}
