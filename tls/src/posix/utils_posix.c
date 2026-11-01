#include "log.h"
#include "utils.h"

#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

// NOLINTBEGIN(concurrency-mt-unsafe)
int8_t generate_random_bytes(u64 num_bytes, u8 *restrict buffer) {
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

void   start_timer(Timer *timer) { clock_gettime(CLOCK_MONOTONIC, &timer->start); }
void   end_timer(Timer *timer) { clock_gettime(CLOCK_MONOTONIC, &timer->end); }
double get_timer_ns(Timer *timer) { return get_clk_ns(&timer->start, &timer->end); }
// NOLINTEND(concurrency-mt-unsafe)
