#ifndef LOG_H
#define LOG_H

#ifndef LOG_LEVEL
#define LOG_LEVEL 0
#endif

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>

#include "platform.h"
#undef stderr

#define LOG_VERBOSE(fmt, ...) (log("\x1b[35m", 0, __FUNCTION__, __LINE__, __FILE__, fmt, __VA_ARGS__))
#define LOG_DEBUG(fmt, ...) (log("\x1b[36m", 1, __FUNCTION__, __LINE__, __FILE__, fmt, __VA_ARGS__))
#define LOG_INFO(fmt, ...) (log("\x1b[32m", 2, __FUNCTION__, __LINE__, __FILE__, fmt, __VA_ARGS__))
#define LOG_WARN(fmt, ...) (log("\x1b[33m", 3, __FUNCTION__, __LINE__, __FILE__, fmt, __VA_ARGS__))
#define LOG_ERROR(fmt, ...) (log("\x1b[31m", 4, __FUNCTION__, __LINE__, __FILE__, fmt, __VA_ARGS__))

// clang-format off
static const char *get_level(int32_t level) {
    switch (level) {
        case 0: return "VERBOSE";
        case 1: return "DEBUG";
        case 2: return "INFO";
        case 3: return "WARN";
        case 4: return "ERROR";
        default: return "VERBOSE";
    }
}
// clang-format on

static inline void PRINTF_LIKE(6, 7)
    log(const char *color, int32_t level, const char *func, int32_t line, const char *const FIL, const char *fmt, ...) {
    if (level < LOG_LEVEL) { return; }

    va_list args;
    va_start(args, fmt);

    // Unlikely for any of these to fail, clang-tidy is paranoid
    (void)fprintf(stderr, "%s[%s]\x1b[0m: [%s:%d:%s]: ", color, get_level(level), FIL, line, func);
    (void)vfprintf(stderr, fmt, args);
    (void)fprintf(stderr, "\n");
    va_end(args);
}

#endif
