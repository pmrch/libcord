#ifndef PLATFORM_H
#define PLATFORM_H

#ifdef _WIN32
#define PACKED __declspec(pack)
#define PRINTF_LIKE(idx, p)

#else
#include <sys/time.h>

#define PACKED __attribute__((packed))
#define PRINTF_LIKE(idx, p) __attribute__((format(printf, idx, p)))
#endif

#endif
