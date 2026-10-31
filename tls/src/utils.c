#include <stdio.h>

#include "log.h"
#include "utils.h"

#define SHIFT32 32

static unsigned char IPV4_BUF_SHARED[IPV4BUF] = {0};

void free_all(Freeable *objs, u64 num) {
    if (objs == NULL) { return; }

    for (u64 i = 0; i < num; i++) {
        Freeable fr_obj = objs[i];
        if (fr_obj.func == NULL || fr_obj.obj == NULL) { continue; }

        LOG_VERBOSE("Freeing %s at %p", fr_obj.name, fr_obj.obj);
        const void *addr = fr_obj.obj;
        fr_obj.func(fr_obj.obj);

        LOG_VERBOSE("Freed object at %p", addr);
        addr = NULL;
    }
}

unsigned char *get_formatted_ipv4(u32 word) {
    memset(IPV4_BUF_SHARED, 0, IPV4BUF); // clang-format off
    (void)snprintf((char *)IPV4_BUF_SHARED, IPV4BUF, "%u.%u.%u.%u", 
        word >> OCT1 & IPV4, word >> OCT2 & IPV4, word >> OCT3 & IPV4, word & IPV4); // clang-format on

    return IPV4_BUF_SHARED;
}

void ipv4_word_to_bytes(u32 word, u8 bytes[4]) {
    bytes[0] = word >> OCT1 & IPV4;
    bytes[1] = word >> OCT2 & IPV4;
    bytes[2] = word >> OCT3 & IPV4;
    bytes[3] = word & IPV4;
}

// Helper to convert hex ASCII character to 4-bit integer
// NOLINTBEGIN(readability-identifier-length, readability-magic-numbers)
static u8 hex_val(char c) {
    if (c >= '0' && c <= '9') { return (u8)(c - '0'); }
    if (c >= 'a' && c <= 'f') { return (u8)(c - 'a' + 10); }
    if (c >= 'A' && c <= 'F') { return (u8)(c - 'A' + 10); }
    return 0;
}

// Convert a 64-character hex string into a 32-byte array (Little-Endian)
void hex_to_bytes_le(const char *hex_str, uint8_t bytes[32]) {
    for (int i = 0; i < 32; i++) {
        // Hex string is big-endian visually; reverse byte index for little-endian byte array
        uint8_t hi = hex_val(hex_str[(u8)(31 - i) * 2]);
        uint8_t lo = hex_val(hex_str[((31 - i) * 2) + 1]);
        bytes[i]   = (u8)(hi << 4) | lo;
    }
}

void hex_to_bytes(const char *hex, u8 *bytes) {
    for (u8 i = 0; i < 32; i++) {
        unsigned int value;
        sscanf(hex + (i * 2), "%2x", &value);
        bytes[i] = (u8)value;
    }
}

// Convert 32 Little-Endian bytes into 5x51-bit limbs
void bytes_to_fe51(const uint8_t bytes[32], u64 *restrict out) {
    // Load 64-bit integer chunks directly using bit shifts
    u64 h0 = (u64)bytes[0] | ((u64)bytes[1] << 8) | ((u64)bytes[2] << 16) | ((u64)bytes[3] << 24) | ((u64)bytes[4] << 32) |
             ((u64)bytes[5] << 40) | ((u64)bytes[6] << 48) | ((u64)bytes[7] << 56);

    u64 h1 = (u64)bytes[6] >> 3 | ((u64)bytes[7] << 5) | ((u64)bytes[8] << 13) | ((u64)bytes[9] << 21) | ((u64)bytes[10] << 29) |
             ((u64)bytes[11] << 37) | ((u64)bytes[12] << 45) | ((u64)bytes[13] << 53);

    u64 h2 = (u64)bytes[12] >> 6 | ((u64)bytes[13] << 2) | ((u64)bytes[14] << 10) | ((u64)bytes[15] << 18) | ((u64)bytes[16] << 26) |
             ((u64)bytes[17] << 34) | ((u64)bytes[18] << 42) | ((u64)bytes[19] << 50);

    u64 h3 = (u64)bytes[19] >> 3 | ((u64)bytes[20] << 5) | ((u64)bytes[21] << 13) | ((u64)bytes[22] << 21) | ((u64)bytes[23] << 29) |
             ((u64)bytes[24] << 37) | ((u64)bytes[25] << 45) | ((u64)bytes[26] << 53);

    u64 h4 = (u64)bytes[25] >> 6 | ((u64)bytes[26] << 2) | ((u64)bytes[27] << 10) | ((u64)bytes[28] << 18) | ((u64)bytes[29] << 26) |
             ((u64)bytes[30] << 34) | ((u64)bytes[31] << 42);

    // Mask each limb to exactly 51 bits (0x7ffffffffffff)
    u64 mask = (1ULL << 51) - 1;

    out[0] = h0 & mask;
    out[1] = h1 & mask;
    out[2] = h2 & mask;
    out[3] = h3 & mask;
    out[4] = h4 & mask;
}

// NOLINTEND(readability-identifier-length, readability-magic-numbers)
