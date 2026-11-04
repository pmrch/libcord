#include "utils.h"
#include "log.h"

#define ONE_HUNDRED 100ULL

// 4 bytes sctrictly as output
static inline void ipv4_word_to_bytes(u32 word, u8 *restrict bytes) {
    bytes[0] = word >> OCT1 & IPV4;
    bytes[1] = word >> OCT2 & IPV4;
    bytes[2] = word >> OCT3 & IPV4;
    bytes[3] = word & IPV4;
}

void free_all(Freeable *objs, u64 num) {
    if (objs == NULL) { return; }

    for (u64 i = 0; i < num; i++) {
        Freeable fr_obj = objs[i];
        if (fr_obj.func == NULL || fr_obj.obj == NULL) { continue; }

        LOG_VERBOSE("Freeing %s at %p", fr_obj.name, fr_obj.obj);
        const void *addr = fr_obj.obj;
        fr_obj.func(fr_obj.obj);

        LOG_VERBOSE("Freed object at %p", addr);
    }
}

// out parameter must be able to hold 16 characters including null-terminator
void get_formatted_ipv4(u32 word, u8 *restrict out) {
    if (word == 0) {
        LOG_ERROR("%s", "Couldn't create formatted ipv4 string, input work was 0");
        return;
    }

    u8 bytes[4];
    ipv4_word_to_bytes(word, bytes);

    // digits of an IPv4 address in ASCII range from 48 to 57
    u8 offset = 0;
    for (u8 i = 0; i < 4; i++) {
        u8 hundr = bytes[i] / ONE_HUNDRED; // clang-format off
        u8 hundreds = ONE_HUNDRED * hundr; // clang-format on

        u8 tens = (bytes[i] - hundreds) / TEN;
        u8 ones = (bytes[i] - (hundreds + (TEN * tens)));

        if (hundr == 0 && tens == 0) {
            out[0 + offset] = '0' + ones;
            out[1 + offset] = '.';
            offset += 2;
        } else if (hundr == 0) {
            out[0 + offset] = '0' + tens;
            out[1 + offset] = '0' + ones;
            out[2 + offset] = '.';
            offset += 3;
        } else {
            out[0 + offset] = '0' + hundr;
            out[1 + offset] = '0' + tens;
            out[2 + offset] = '0' + ones;
            out[3 + offset] = '.';
            offset += 4;
        }
    }

    out[offset - 1] = '\0';
}
