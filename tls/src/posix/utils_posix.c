#include "log.h"
#include "utils.h"

#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

#define SHIFT_LOWER 8
#define MAX_BYTES 32

// NOLINTBEGIN(concurrency-mt-unsafe)
static int8_t generate_random_bytes(u64 num_bytes, u8 *buffer) {
    if (buffer == NULL) {
        LOG_ERROR("%s", "Cannot generate random into a NULL buffer!");
        return -1;
    }

    int32_t f_desc = open("/dev/urandom", O_CLOEXEC | O_RDONLY);
    if (f_desc < 0) {
        LOG_ERROR("Failed to open urandom: %s", strerror(errno));
        return -1;
    }

    int64_t read_bytes = read(f_desc, buffer, num_bytes); // clang-format on
    close(f_desc);

    if (read_bytes < 0) {
        LOG_ERROR("Failed to read urandom: %s", strerror(errno));
        return -1;
    }

    return 0;
}

u16 generate_random_u16(void) {
    u8 bytes[2] = {0};
    generate_random_bytes(2, bytes);

    int32_t result = ((u16)bytes[0] << SHIFT_LOWER) | (u16)bytes[1];
    return (u16)result % (UINT16_MAX + 1);
}
// NOLINTEND(concurrency-mt-unsafe)
