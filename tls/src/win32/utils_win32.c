#ifdef _MSC_VER

#include "log.h"
#include "platform.h"
#include "utils.h"

#include <Windows.h>
#include <bcrypt.h>
#pragma comment(lib, "bcrypt.lib")

int8_t generate_random_bytes(u64 num_bytes, u8 *restrict buffer) {
    NTSTATUS status = BCryptGenRandom(NULL, buffer, (ULONG)num_bytes, BCRYPT_USE_SYSTEM_PREFERRED_RNG);

    if (status != 0) {
        LOG_ERROR("Failed to generate %lu random bytes! NTSTATUS: 0x%08X", num_bytes, status);
        return -1;
    }

    LOG_DEBUG("Successfully generated crptographically safe %lu random bytes", num_bytes);
    return 0;
}

void start_timer(Timer *timer) {
    QueryPerformanceFrequency(&timer->freq);
    QueryPerformanceCounter(&timer->start);
}

void end_timer(Timer *timer) { QueryPerformanceCounter(&timer->end); }

double get_timer_ns(Timer *timer) {
    LARGE_INTEGER elapsed = {0};
    elapsed.QuadPart      = timer->end.QuadPart - timer->start.QuadPart;
    double result         = ((double)elapsed.QuadPart) / ((double)timer->freq.QuadPart);
    return result * ONE_BILLION;
}

#else
__attribute__((unused)) static void dont_flag(void);
#endif
