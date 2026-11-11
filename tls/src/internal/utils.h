#ifndef UTILS_H
#define UTILS_H

#include "constants.h"
#include "types.h"
#include <string.h>

static const char HEX_TABLE[BITS16]       = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
static const char HEX_TABLE_UPPER[BITS16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

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

i8 generate_random_bytes(u64 num_bytes, u8 *restrict buffer);

static inline u8 buf_to_hex_ns(const u8 *buffer, u64 buflen, char *restrict hexbuf, u64 hexbuf_len) {
    memset(hexbuf, 0, hexbuf_len);
    if ((buflen * 2) + 1 > hexbuf_len) { return FALSE; }

    uint64_t local_off = 0;
    for (uint64_t i = 0; i < buflen; i++) {
        uint8_t cur_byte = buffer[i];

        hexbuf[local_off++] = HEX_TABLE[(cur_byte >> 4) & MASK_NIBBLE];
        hexbuf[local_off++] = HEX_TABLE[cur_byte & MASK_NIBBLE];
    }

    hexbuf[local_off] = '\0';
    return TRUE;
}

static inline u8 buf_to_hex_upper(const u8 *buffer, u64 buflen, char *restrict hexbuf, u64 hexbuf_len) {
    memset(hexbuf, 0, hexbuf_len);
    if ((buflen * 3) + 1 > hexbuf_len) { return FALSE; }

    uint64_t local_off = 0;
    for (uint64_t i = 0; i < buflen; i++) {
        uint8_t cur_byte = buffer[i];

        hexbuf[local_off++] = HEX_TABLE_UPPER[(cur_byte >> 4) & MASK_NIBBLE];
        hexbuf[local_off++] = HEX_TABLE_UPPER[cur_byte & MASK_NIBBLE];
        hexbuf[local_off++] = ' ';
    }

    hexbuf[local_off] = '\0';
    return TRUE;
}

static inline u16 generate_random_u16(void) {
    u8 bytes[2] = {0};
    generate_random_bytes(2, bytes);
    i32 result = ((u16)bytes[0] << ONE_BYTE) | (u16)bytes[1];
    return (u16)result;
}

// Converts 8 Little Endian bytes into 64-bit unsigned integer
static inline u64 u64_from_bytes_le(const u8 *restrict bytes) {
    u64 result = (u64)bytes[0];
    for (u8 i = 1; i < ONE_BYTE; i++) { result |= ((u64)bytes[i] << (ONE_BYTE * i)); }
    return result;
}

// Converts 64-bit unsigned integer into 8 Little Endian bytes
static inline void u64_to_bytes(u64 value, u8 *restrict out) {
    for (u8 i = 1; i < ONE_BYTE; i++) { out[i] = (u8)(value >> (ONE_BYTE * i)) & MASK_BYTE; }
    out[0] = value & MASK_BYTE;
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
    return (((val & 0x00000000000000FFULL) << 56) | ((val & 0x000000000000FF00ULL) << 40) | ((val & 0x0000000000FF0000ULL) << 24) |
            ((val & 0x00000000FF000000ULL) << 8) | ((val & 0x000000FF00000000ULL) >> 8) | ((val & 0x0000FF0000000000ULL) >> 24) |
            ((val & 0x00FF000000000000ULL) >> 40) | ((val & 0xFF00000000000000ULL) >> 56));
}
// NOLINTEND (readability-magic-numbers)

#endif
