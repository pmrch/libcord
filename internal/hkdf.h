#ifndef CDISCORD_HKDF_H
#define CDISCORD_HKDF_H

#include "types.h"
#include <string.h>

#define HASH_LEN 32

typedef struct {
    u16 length;
    u8  label[UINT8_MAX];
    u8  context[UINT8_MAX];
} HkdfLabel;

void hkdf_extract(const u8 *restrict salt, u64 salt_len, u8 *restrict out, u64 out_len, const u8 *restrict ikm, u64 ikm_len);
void hkdf_expand(const u8 *restrict rand_key, u64 rand_len, u8 *restrict key_out, u64 outlen, const u8 *restrict info, u64 info_len);

static inline HkdfLabel create_label(const char *restrict label, u16 length, const u8 *restrict context, u64 context_len) {
    HkdfLabel final_label = {0};

    final_label.length = length;
    u64 label_len      = sizeof("tls13 ") - 1;

    memcpy(final_label.label, "tls13 ", label_len);
    memcpy(final_label.label + label_len, label, strlen(label));
    memcpy(final_label.context, context, context_len);

    return final_label;
}

#endif
