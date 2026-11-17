#include "hkdf.h"
#include "log.h"
#include "sha256.h"

#include <stdlib.h>
#include <string.h>

#define HMAC_MAX (32 + 255 + 1)

static inline void copy_to_local(const char *var, const u8 *restrict input, u64 inlen, u8 *restrict out, u64 outlen) {
    const char *local_var = var == NULL ? "general" : var;
    if (out == NULL || outlen == 0) {
        LOG_ERROR("Output buffer was of 0 length or NULL for %s buffer", local_var);
        return;
    }

    if (input != NULL && inlen != 0) {
        if (inlen > outlen) {
            LOG_WARN("Not copying %s to local buffer, salt more than %zu bytes. Leaving zeros!", local_var, outlen);
            memset(out, 0, outlen);
            return;
        }

        memcpy(out, input, inlen);
    }
}

void hkdf_extract(const u8 *restrict salt, u64 salt_len, u8 *restrict out, u64 out_len, const u8 *restrict ikm, u64 ikm_len) {
    u8 local_salt[HASH_LEN * 4]; // Potential support for SHA512

    if (ikm == NULL || ikm_len == 0) {
        LOG_ERROR("%s", "Compeltely invalid input key material provided!");
        return;
    }

    if (out_len < HASH_LEN) {
        LOG_ERROR("Too small output buffer: %llu!", (unsigned long long)out_len);
        return;
    }

    copy_to_local("salt", salt, salt_len, local_salt, sizeof(local_salt));
    hmac_sha256(local_salt, salt_len, ikm, ikm_len, out);
}

void hkdf_expand(const u8 *restrict rand_key, u64 rand_len, u8 *restrict key_out, u64 outlen, const u8 *restrict info, u64 info_len) {
    u32 done    = 0;
    u8  counter = 1;

    u8 temp_hmac[HASH_LEN];
    while (done < outlen) {
        u8  hmac_data[HMAC_MAX];
        u64 data_len = 0;

        // If this isn't the first iteration, append the previous T
        if (counter > 1) {
            memcpy(hmac_data + data_len, temp_hmac, HASH_LEN);
            data_len += HASH_LEN;
        }

        // Append the info
        memcpy(hmac_data + data_len, info, info_len);
        data_len += info_len;

        // Append counter
        hmac_data[data_len++] = counter;
        hmac_sha256(rand_key, rand_len, hmac_data, data_len, temp_hmac);

        u64 to_copy = outlen - done;
        if (to_copy > HASH_LEN) { to_copy = HASH_LEN; }
        memcpy(key_out + done, temp_hmac, to_copy);

        done += to_copy;
        counter++;
    }
}
