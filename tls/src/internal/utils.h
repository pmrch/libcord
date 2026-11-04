#ifndef UTILS_H
#define UTILS_H

#include "constants.h"
#include "types.h"


#if !defined(__SIZEOF_INT128__)
u128 u128_add(u128 left, u128 right);
u128 u128_add_u64(u128 num_a, u64 num_b);

u128 u128_shr51(u128 num_a);


#else
static inline u128 mul_u64(u64 term1, u64 term2) { return (u128)term1 * (u128)term2; }
static inline u128 u128_add(u128 term1, u128 term2) { return term1 + term2; }
static inline u64  u128_shr51(u128 term1) { return (u64)(term1 >> LIMB_SIZE); }

#endif

void free_all(Freeable *objs, u64 num);

// out must be at least be able to contain characters xxx.xxx.xxx.xxx
// so it must be able to hold 16 characters including null-terminator
void get_formatted_ipv4(u32 word, u8 *restrict out);

int8_t generate_random_bytes(u64 num_bytes, u8 *restrict buffer);

static inline u16 generate_random_u16(void) {
    u8 bytes[2] = {0};
    generate_random_bytes(2, bytes);
    int32_t result = ((u16)bytes[0] << ONE_BYTE) | (u16)bytes[1];
    return (u16)result;
}

static inline u64 u64_from_bytes_le(const u8 bytes[ONE_BYTE]) {
    u64 result = (u64)bytes[0];
    for (u8 i = 1; i < ONE_BYTE; i++) { result |= ((u64)bytes[i] << (ONE_BYTE * i)); }
    return result;
}

static inline void u64_to_bytes(u64 value, u8 out[ONE_BYTE]) {
    for (u8 i = 1; i < ONE_BYTE; i++) { out[i] = (u8)(value >> (ONE_BYTE * i)) & IPV4; }
    out[0] = value & IPV4;
}

// Helper to convert hex ASCII character to 4-bit integer
static inline u8 hex_val(char ascii_char) {
    if (ascii_char >= '0' && ascii_char <= '9') { return (u8)(ascii_char - '0'); }
    if (ascii_char >= 'a' && ascii_char <= 'f') { return (u8)((u8)(ascii_char - 'a') + ASCII_OFFSET); }
    if (ascii_char >= 'A' && ascii_char <= 'F') { return (u8)((u8)(ascii_char - 'A') + ASCII_OFFSET); }
    return 0;
}

// Convert a 64-character hex string into a 32-byte array (Little-Endian)
// output parameter bytes must be exactly 32-bytes big
static inline void hex_to_bytes(const char *restrict hex, u8 *restrict bytes) {
    for (u8 i = 0; i < BITS32; i++) {
        u8 high  = hex_val(hex[(u64)i * 2]);
        u8 low   = hex_val(hex[((u64)i * 2) + 1]);
        bytes[i] = (u8)((high << 4) | low);
    }
}

// NOLINTBEGIN (readability-magic-numbers)
// AI-generated utility for turning little endian into big endian
static inline u64 swap_u64(u64 val) {
    return (((val & 0x00000000000000FFULL) << 56) |
     ((val & 0x000000000000FF00ULL) << 40) |
     ((val & 0x0000000000FF0000ULL) << 24) |
     ((val & 0x00000000FF000000ULL) << 8)  |
     ((val & 0x000000FF00000000ULL) >> 8)  |
     ((val & 0x0000FF0000000000ULL) >> 24) |
     ((val & 0x00FF000000000000ULL) >> 40) |
     ((val & 0xFF00000000000000ULL) >> 56)
    );
}
// NOLINTEND (readability-magic-numbers)

#endif
