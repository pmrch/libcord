#include <stdio.h>

#include "log.h"
#include "utils.h"

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
    return ((u16)((buffer[0] << (1 << 3)) | buffer[1]) % (UINT16_MAX + 1));
}
