#include <stdio.h>
#include <string.h>

#include "log.h"
#include "utils.h"

#define MASK32 0xFFFFFFFF
#define SHIFT32 32

static unsigned char IPV4_BUF_SHARED[IPV4BUF] = {0};

void free_all(Freeable *objs, u64 num) {
    if (objs == NULL) { return; }

#pragma unroll 4
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

u16 generate_random_u16(void) {
    FILE *rand = fopen("/dev/urandom", "rbe");
    if (rand == NULL) {
        LOG_ERROR("%s", "Failed to open /dev/urandom");
        return 0;
    }

    u8  buffer[2]  = {0};
    u64 read_bytes = fread(buffer, 1, 2, rand);

    (void)read_bytes, fclose(rand);
    return ((u16)((buffer[0] << ((1 << 3))) | buffer[1]) % (UINT16_MAX + 1));
}

unsigned char *get_formatted_ipv4(u32 word) {
    memset(IPV4_BUF_SHARED, 0, IPV4BUF); // clang-format off
    (void)snprintf((char *)IPV4_BUF_SHARED, IPV4BUF, "%u.%u.%u.%u", 
        word >> OCT1 & IPV4, word >> OCT2 & IPV4, word >> OCT3 & IPV4, word & IPV4); // clang-format on

    return IPV4_BUF_SHARED;
}

u64 u64_from_bytes_le(const u8 bytes[ONE_BYTE]) {
    u64 result = (u64)bytes[0];

#pragma unroll
    for (u8 i = 1; i < ONE_BYTE; i++) { result |= (u64)(bytes[i] << (ONE_BYTE * i)); }
    return result;
}

void u64_to_bytes(u64 value, u8 out[ONE_BYTE]) {
#pragma unroll
    for (u8 i = 1; i < ONE_BYTE; i++) { out[i] = (u8)(value >> (ONE_BYTE * i)) & IPV4; }
    out[0] = value & IPV4;
}

void ipv4_word_to_bytes(u32 word, u8 bytes[4]) {
    bytes[0] = word >> OCT1 & IPV4;
    bytes[1] = word >> OCT2 & IPV4;
    bytes[2] = word >> OCT3 & IPV4;
    bytes[3] = word & IPV4;
}

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
pseudo_u128 u128_add(pseudo_u128 left, pseudo_u128 right) {
    pseudo_u128 result = {0};

    result.low_bits  = left.low_bits + right.low_bits;
    result.high_bits = left.high_bits + right.high_bits + (result.low_bits < left.low_bits ? 1 : 0);

    return result;
}

pseudo_u128 u128_add_u64(pseudo_u128 num_a, u64 num_b) {
    pseudo_u128 result = {0};

    result.low_bits  = num_a.low_bits + num_b;
    result.high_bits = num_a.high_bits + (result.low_bits < num_a.low_bits ? 1 : 0);

    return result;
}

u64 u128_shr51(pseudo_u128 num_a) { return (num_a.low_bits >> LIMB_SIZE) | (num_a.high_bits << LIMB_STEP); }

// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
pseudo_u128 mul_u64(u64 left, u64 right) {
    u64 a_low = left & MASK32;
    u64 b_low = right & MASK32;

    u64 a_high = left >> SHIFT32;
    u64 b_high = right >> SHIFT32;

    u64 low_low   = a_low * b_low;
    u64 low_high  = a_low * b_high;
    u64 high_low  = a_high * b_low;
    u64 high_high = a_high * b_high;

    // mid is the cross terms combined
    // clang-format off
    u64 mid = (low_low >> SHIFT32) + (low_high & MASK32) + high_low;
    pseudo_u128 result = {
        .low_bits  = (low_low & MASK32) | (mid << SHIFT32),
        .high_bits = high_high + (low_high >> SHIFT32) + (mid >> SHIFT32)
    }; // clang-format on

    return result;
}
