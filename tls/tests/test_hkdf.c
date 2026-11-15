#include "hkdf.h"
#include "utils.h"

#include <stdio.h>
#include <string.h>

// Helper to parse hex strings into byte arrays
static size_t parse_hex(const char *hex, u8 *out, size_t max_len) {
    size_t len = strlen(hex) / 2;
    if (len > max_len) return 0;
    for (size_t i = 0; i < len; i++) {
        unsigned int byte;
        (void)sscanf(hex + i * 2, "%02x", &byte);
        out[i] = (u8)byte;
    }
    return len;
}

// Helper to compare and print results
static int check_result(const char *test_name, const u8 *actual, const char *expected_hex) {
    char actual_hex[256] = {0};
    for (size_t i = 0; i < strlen(expected_hex) / 2; i++) { (void)sprintf(actual_hex + (i * 2), "%02x", actual[i]); }

    if (strcmp(actual_hex, expected_hex) == 0) {
        printf("[PASS] %s\n", test_name);
        return 0;
    }

    printf("[FAIL] %s\n", test_name);
    printf("  Expected: %s\n", expected_hex);
    printf("  Actual:   %s\n", actual_hex);
    return 1;
}

// NOLINTBEGIN
int main(void) {
    printf("=== Running RFC 5869 HKDF-SHA256 Tests ===\n");
    int fails = 0;

    // ---------------------------------------------------------
    // Test Case 1: Basic
    // ---------------------------------------------------------
    u8 ikm1[22];
    u8 salt1[13];
    u8 info1[10];

    parse_hex("0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b", ikm1, 22);
    parse_hex("000102030405060708090a0b0c", salt1, 13);
    parse_hex("f0f1f2f3f4f5f6f7f8f9", info1, 10);

    u8 prk1[32], okm1[42];
    hkdf_extract(salt1, 13, prk1, 32, ikm1, 22);
    fails += check_result("TC1: Extract (PRK)", prk1, "077709362c2e32df0ddc3f0dc47bba6390b6c73bb50f9c3122ec844ad7c2b3e5");

    hkdf_expand(prk1, 32, okm1, 42, info1, 10);
    fails += check_result("TC1: Expand (OKM)", okm1,
                          "3cb25f25faacd57a90434f64d0362f2a2d2d0a90cf1a5a4c5db02d56ecc4c5bf34007208d5b887185865");

    // ---------------------------------------------------------
    // Test Case 2: Long Inputs (Tests multi-block expand, N=3)
    // ---------------------------------------------------------
    u8 ikm2[80], salt2[80], info2[80];
    // Fill IKM with 0x00 to 0x4F
    for (int i = 0; i < 80; i++) ikm2[i] = (u8)i;
    // Fill Salt with 0x60 to 0xAF
    for (int i = 0; i < 80; i++) salt2[i] = (u8)(i + 0x60);
    // Fill Info with 0xB0 to 0xFE
    for (int i = 0; i < 80; i++) info2[i] = (u8)(i + 0xB0);

    u8 prk2[32], okm2[82];
    hkdf_extract(salt2, 80, prk2, 32, ikm2, 80);
    fails += check_result("TC2: Extract (PRK)", prk2, "06a6b88c5853361a06104c9ceb35b45cef760014904671014a193f40c15fc244");

    hkdf_expand(prk2, 32, okm2, 82, info2, 80);
    fails += check_result("TC2: Expand (OKM)", okm2,
                          "b11e398dc80327a1c8e7f78c596a49344f012eda2d4efad8a050cc4c19afa97c"
                          "59045a99cac7827271cb41c65e590e09da3275600c2f09b8367793a9aca3db71"
                          "cc30c58179ec3e87c14c01d5c1f3434f1d87");

    // ---------------------------------------------------------
    // Test Case 3: Zero-length salt and info
    // ---------------------------------------------------------
    u8 ikm3[22];
    parse_hex("0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b", ikm3, 22);

    u8 prk3[32], okm3[42];
    // Pass NULL for salt, 0 for length
    hkdf_extract(NULL, 0, prk3, 32, ikm3, 22);
    fails += check_result("TC3: Extract (PRK)", prk3, "19ef24a32c717b167f33a91d6f648bdf96596776afdb6377ac434c1c293ccb04");

    // Pass NULL for info, 0 for length
    hkdf_expand(prk3, 32, okm3, 42, NULL, 0);
    fails += check_result("TC3: Expand (OKM)", okm3,
                          "8da4e775a563c18f715f802a063c5a31b8a11f5c5ee1879ec3454e5f3c738d2d9d201395faa4b61a96c8");

    if (fails == 0) {
        printf("All HKDF-SHA256 tests passed!\n");
        return 0;
    }

    printf("%d HKDF tests FAILED!\n", fails);
    return 1;
}
// NOLINTEND
