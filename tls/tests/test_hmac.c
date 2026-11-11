#include "sha256.h" // Adjust include path if needed
#include <stdio.h>
#include <string.h>

// Helper to print hex for debugging
// NOLINTBEGIN (readability-magic-numbers)
static void print_hex(const u8 *buf, size_t len) {
    for (size_t i = 0; i < len; i++) { printf("%02x", buf[i]); }
    printf("\n");
}

// Helper to compare expected and actual
static int check_hmac(const char *test_name, const u8 *key, size_t key_len, const u8 *data, size_t data_len,
                      const char *expected_hex) {
    u8 out[32];
    hmac_sha256(key, key_len, data, data_len, out);

    char out_hex[65] = {0};
    for (int i = 0; i < 32; i++) { (void)(sprintf(out_hex + (i * 2), "%02x", out[i])); }

    if (strcmp(out_hex, expected_hex) == 0) {
        printf("\x1b[32m[PASS] %s\x1b[0m\n", test_name);
        return 0;
    } else {
        printf("\x1b[31m[FAIL] %s\x1b[0m\n", test_name);
        printf("  Expected: %s\n", expected_hex);
        printf("  Actual:   %s\n", out_hex);
        return 1;
    }
}

static void test_hmac_sha256(void) {
    printf("=== Running RFC 4231 HMAC-SHA256 Tests ===\n");
    int fails = 0;

    // Test Case 1: Basic
    u8          key1[20] = {0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b,
                            0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b, 0x0b};
    const char *data1    = "Hi There";
    fails += check_hmac("TC1: Basic", key1, 20, (const u8 *)data1, strlen(data1),
                        "b0344c61d8db38535ca8afceaf0bf12b881dc200c9833da726e9376c2e32cff7");

    // Test Case 2: Key shorter than block size, data is "what do ya want for nothing?"
    const char *key2  = "Jefe";
    const char *data2 = "what do ya want for nothing?";
    fails += check_hmac("TC2: 'Jefe' key", (const u8 *)key2, strlen(key2), (const u8 *)data2, strlen(data2),
                        "5bdcc146bf60754e6a042426089575c75a003f089d2739839dec58b964ec3843");

    // Test Case 3: 20-byte key of 0xaa, 50 bytes of 0xdd
    u8 key3[20];
    memset(key3, 0xaa, 20);
    u8 data3[50];
    memset(data3, 0xdd, 50);
    fails += check_hmac("TC3: 0xaa key / 0xdd data", key3, 20, data3, 50,
                        "773ea91e36800e46854db8ebd09181a72959098b3ef8c122d9635514ced565fe");

    // Test Case 4: 25-byte key, 50 bytes of 0xcd
    u8 key4[25];
    for (int i = 0; i < 25; i++) key4[i] = (u8)(i + 1);
    u8 data4[50];
    memset(data4, 0xcd, 50);
    fails += check_hmac("TC4: Sequence key", key4, 25, data4, 50, "82558a389a443c0ea4cc819899f2083a85f0faa3e578f8077a2e3ff46729665b");

    // Test Case 6: Key larger than block size (131 bytes of 0xaa) - THIS TESTS THE HASHING LOGIC
    u8 key6[131];
    memset(key6, 0xaa, 131);
    const char *data6 = "Test Using Larger Than Block-Size Key - Hash Key First";
    fails += check_hmac("TC6: Key > 64 bytes", key6, 131, (const u8 *)data6, strlen(data6),
                        "60e431591ee0b67f0d8a26aacbf5b77f8e0bc6213728c5140546040f0ee37f54");

    // Test Case 7: Key larger than block size, data larger than block size
    u8 key7[131];
    memset(key7, 0xaa, 131);
    const char *data7 = "This is a test using a larger than block-size key and a larger than block-size data. The key needs to be "
                        "hashed before being used by the HMAC algorithm.";
    fails += check_hmac("TC7: Key & Data > 64 bytes", key7, 131, (const u8 *)data7, strlen(data7),
                        "9b09ffa71b942fcb27635fbcd5b0e944bfdc63644f0713938a7f51535c3a35e2");

    if (fails == 0) {
        printf("\x1b[32mAll HMAC-SHA256 tests passed!\x1b[0m\n");
    } else {
        printf("\x1b[31m%d HMAC tests FAILED!\x1b[0m\n", fails);
    }
}

// NOLINTEND (readability-magic-numbers)

int main(void) {
    test_hmac_sha256();
    return 0;
}
