#include "crypt.h"
#include "curve25519_donna.h"
#include "platform.h"
#include "utils.h"

#include <stdio.h>
#include <string.h>

#define CLR_GREEN "\033[32m"
#define CLR_RED   "\033[31m"
#define CLR_RESET "\033[0m"

#ifndef ITER_COUNT
#define ITER_COUNT 10000
#endif

typedef struct {
    const char *scalar;
    const char *ucoord;
    const char *expected;
} X25519TestVector;

// clang-format off
static const X25519TestVector VECTORS[] = {
    {
        .scalar   = "a546e36bf0527c9d3b16154b82465edd62144c0ac1fc5a18506a2244ba449ac4",
        .ucoord   = "e6db6867583030db3594c1a424b15f7c726624ec26b3353b10a903a6d0ab1c4c",
        .expected = "c3da55379de9c6908e94ea4df28d084f32eccf03491c71f754b4075577a28552",
    },
    {
        .scalar   = "4b66e9d4d1b4673c5ad22691957d6af5c11b6421e0ea01d42ca4169e7918ba0d",
        .ucoord   = "e5210f12786811d3f4b7959d0538ae2c31dbe7106fc03c3efc4cd549c715a493",
        .expected = "95cbde9476e8907d7aade45cb4b873f88b595a68799fa152e6f8f7647aac7957",
    },

    // Alice's public key
    {
        .scalar   = "77076d0a7318a57d3c16c17251b26645df4c2f87ebc0992ab177fba51db92c2a",
        .ucoord   = "0900000000000000000000000000000000000000000000000000000000000000",
        .expected = "8520f0098930a754748b7ddcb43ef75a0dbf3a0d26381af4eba4a98eaa9b4e6a",
    },

    // Bob's public key
    {
        .scalar   = "5dab087e624a8a4b79e17f8b83800ee66f3bb1292618b6fd1c2f8b27ff88e0eb",
        .ucoord   = "0900000000000000000000000000000000000000000000000000000000000000",
        .expected = "de9edb7d7b7dc1b4d35b61c2ece435373f8343c85b78674dadfc7e146f882b4f",
    },

    // RFC 7748 iterative test (first iteration)
    {
        .scalar   = "0900000000000000000000000000000000000000000000000000000000000000",
        .ucoord   = "0900000000000000000000000000000000000000000000000000000000000000",
        .expected = "422c8e7a6227d7bca1350b3e2bb7279f7897b87bb6854b783c60e80311ae3079",
    },
};

// clang-format on
static inline void print_hex(const u8 *bytes) {
    for (u8 k = 0; k < BITS32; k++) { printf("%02x", bytes[k]); }
    printf("\n");
}

static inline void test_predefined(void) {
    // Both official test vector cases from RFC 7748 for X25519
    int num_vectors  = sizeof(VECTORS) / sizeof(VECTORS[0]);
    int failed_count = 0;

    for (int i = 0; i < num_vectors; i++) {
        u8 bytes_scalar[BITS32];
        u8 bytes_ucoord[BITS32];
        u8 expected[BITS32];

        u8   output[BITS32] = {0};
        fe51 field_output   = {0};

        // Convert hex inputs and expected output to byte arrays
        hex_to_bytes(VECTORS[i].scalar, bytes_scalar);
        hex_to_bytes(VECTORS[i].ucoord, bytes_ucoord);
        hex_to_bytes(VECTORS[i].expected, expected);

        // Execute curve25519 and encode result
        curve25519(bytes_scalar, bytes_ucoord, field_output);
        encode_u_coordinate(output, field_output);

        // Validate correctness
        if (memcmp(output, expected, BITS32) != 0) {
            printf("%sX25519 test vector %d FAILED!%s\n", CLR_RED, i + 1, CLR_RESET);

            printf("Expected: ");
            print_hex(expected);

            printf("Actual: ");
            print_hex(output);

            printf("\n");
            failed_count++;
        } else {
            printf("%sX25519 test vector %d PASSED.%s\n", CLR_GREEN, i + 1, CLR_RESET);
        }
    }

    if (failed_count == 0) {
        printf("\n%sAll %d X25519 test VECTORS PASSED successfully.%s\n", CLR_GREEN, num_vectors, CLR_RESET);
    } else {
        printf("\n%s%d of %d X25519 test VECTORS FAILED.%s\n", CLR_RED, failed_count, num_vectors, CLR_RESET);
    }
}

static inline void test_random(u64 num_iter) {
    printf("\n=== Starting Differential Fuzz Testing against curve25519-donna ===");
    Timer timer = {0};

    u8 random_scalar[BITS32] = {0};
    u8 random_ucoord[BITS32] = {0};

    u8 my_output[BITS32]    = {0};
    u8 donna_output[BITS32] = {0};

    fe51 my_field_elem = {0};
    int  diff_failed   = 0;

    start_timer(&timer);
    for (u64 i = 0; i < num_iter; i++) {
        generate_random_bytes(BITS32, random_scalar);
        generate_random_bytes(BITS32, random_ucoord);

        curve25519(random_scalar, random_ucoord, my_field_elem);
        encode_u_coordinate(my_output, my_field_elem);

        i64 donna_ret = curve25519_donna(donna_output, random_scalar, random_ucoord);
        if (donna_ret != 0) { continue; }

        if (memcmp(my_output, donna_output, BITS32) != 0) {
            printf("Fuzz test FAILED on iteration %llu!\n", i + 1);

            printf("Scalar:     ");
            print_hex(random_scalar);

            printf("\nBasepoint:  ");
            print_hex(random_ucoord);

            printf("\nMy Output:  ");
            print_hex(my_output);

            printf("\nDonna Output: ");
            print_hex(donna_output);

            printf("\n\n");

            diff_failed++;
            break; // Stop on first failure to debug easily
        }
    }

    end_timer(&timer);
    double ns = get_timer_ns(&timer);

    if (diff_failed == 0) {
        printf("\n%sAll %llu X25519 randomly generated test VECTORS PASSED successfully.%s\n", CLR_GREEN, num_iter, CLR_RESET);
    } else {
        printf("\n%s%d of %llu X25519 randomly generated test VECTORS FAILED.%s\n", CLR_RED, diff_failed, num_iter, CLR_RESET);
    }

    printf("\nTotal time: %f ns\n", ns);
    printf("Average: %.2f us/op\n", ns / 10000.0 / 1000.0);
}

int main(void) {
    test_predefined();
    test_random(ITER_COUNT);
    return 0;
}
