#ifndef CDISCORD_HKDF_H
#define CDISCORD_HKDF_H

#include "types.h"

#define HASH_LEN 32

void hkdf_extract(const u8 *restrict salt, u64 salt_len, u8 *restrict out, u64 out_len, const u8 *restrict ikm, u64 ikm_len);
void hkdf_expand(const u8 *restrict rand_key, u64 rand_len, u8 *restrict key_out, u64 outlen, const u8 *restrict info, u64 info_len);
void hkdf_expand_label(const u8 *secret, u64 secret_len, const char *label, const u8 *context, u64 context_len, u8 *out, u64 out_len);

#endif
