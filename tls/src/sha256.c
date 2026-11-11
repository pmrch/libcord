#include "sha256.h"
#include "constants.h"
#include "log.h"

#include <string.h>

// Constant message schedula index offsets
#define PADDING 0x80
#define FIFTY_SIX ((2 * THREE_BYTE) + ONE_BYTE)

#define IPAD 0x36
#define OPAD 0x5c

void sha256_init(sha256_ctx *restrict ctx) {
    if (ctx == NULL) {
        LOG_ERROR("%s", "Context cannot be initialized, it was NULL!");
        return;
    }

    // clang-format off
    memset(ctx->buffer, 0, sizeof(ctx->buffer));
    ctx->h[0] = HASH_TABLE[0]; ctx->h[1] = HASH_TABLE[1]; ctx->h[2] = HASH_TABLE[2]; ctx->h[3] = HASH_TABLE[3]; 
    ctx->h[4] = HASH_TABLE[4]; ctx->h[FIVE] = HASH_TABLE[FIVE]; ctx->h[SIX] = HASH_TABLE[SIX]; ctx->h[SEVEN] = HASH_TABLE[SEVEN];
    // clang-format on

    ctx->buffer_len = 0;
    ctx->length     = 0;
}

void sha256_update(sha256_ctx *restrict ctx, const u8 *restrict data, size_t len) {
    ctx->length += (u64)(len * ONE_BYTE);

    // If we have buffered data, fill it up
    while (len > 0 && ctx->buffer_len > 0) {
        ctx->buffer[ctx->buffer_len++] = *data++;
        len--;

        if (ctx->buffer_len == SHA256_BLOCK_SIZE) {
            sha256_compress(ctx, ctx->buffer);
            ctx->buffer_len = 0;
        }
    }

    // Compress directly from the data pointer if we have full blocks
    while (len >= SHA256_BLOCK_SIZE) {
        sha256_compress(ctx, data);
        data += SHA256_BLOCK_SIZE;
        len -= SHA256_BLOCK_SIZE;
    }

    // Buffer any remaining bytes
    while (len > 0) {
        ctx->buffer[ctx->buffer_len++] = *data++;
        len--;
    }
}

void sha256_final(sha256_ctx *restrict ctx, u8 *restrict out) {
    u32 index = ctx->buffer_len;

    // Pad with 0x80
    ctx->buffer[index++] = PADDING;

    // If we don't have enough room for the 8-byte length, pad and compress
    if (index > FIFTY_SIX) {
        while (index < (FOUR_BYTE * 2)) { ctx->buffer[index++] = 0x00; }
        sha256_compress(ctx, ctx->buffer);
        index = 0;
    }

    // Pad with zeros until byte 56
    while (index < FIFTY_SIX) { ctx->buffer[index++] = 0x00; }

    // Append the length in bits as a 64-bit Big-Endian integer
    u8 offset = 0;
    for (u8 i = FIFTY_SIX; i < (u8)(FOUR_BYTE * 2) - 1; i++) {
        ctx->buffer[i] = (u8)(ctx->length >> (FIFTY_SIX - (offset * ONE_BYTE)));
        offset++;
    }

    // Compress the final block
    ctx->buffer[(u8)(FOUR_BYTE * 2) - 1] = (u8)(ctx->length);
    sha256_compress(ctx, ctx->buffer);

    // Store output hash as Big Endian, guaranteed 32 bytes final
    for (int j = 0; j < (u8)ONE_BYTE; j++) { store_be32(out + (u16)(j * 4), ctx->h[j]); }
}

void hmac_sha256(const u8 *key, size_t key_len, const u8 *data, size_t data_len, u8 *restrict out) {
    u8 k_ipad[BITS64];
    u8 k_opad[BITS64];
    u8 inner_hash[BITS32];

    sha256_ctx ctx = {0};
    sha256_init(&ctx);
    u8 local_key[BITS64] = {0};

    if (key_len > BITS64) {
        sha256_update(&ctx, key, key_len);
        sha256_final(&ctx, local_key);
    } else {
        memcpy(local_key, key, key_len);
    }

    for (u8 i = 0; i < BITS64; i++) {
        k_ipad[i] = local_key[i] ^ IPAD;
        k_opad[i] = local_key[i] ^ OPAD;
    }

    // reinitialize so that we can perform inner hash calculations unaffected
    sha256_init(&ctx);

    // Inner hash: H(k_ipad || data)
    sha256_update(&ctx, k_ipad, sizeof(k_ipad));
    sha256_update(&ctx, data, data_len);
    sha256_final(&ctx, inner_hash);

    // Outer hash: H(k_opad || inner_hash)
    sha256_init(&ctx); // Reinit for outer hash calculation
    sha256_update(&ctx, k_opad, sizeof(k_opad));
    sha256_update(&ctx, inner_hash, sizeof(inner_hash));
    sha256_final(&ctx, out);
}
