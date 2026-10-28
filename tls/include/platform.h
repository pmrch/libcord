#ifndef PLATFORM_H
#define PLATFORM_H

#ifdef _WIN32
#define PACKED __declspec(pack)
#define ALIGN(x) __declspec(align(x))
#define PRINTF_LIKE(idx, p)
#else
#define PACKED __attribute__((packed))
#define ALIGN(x) __attribute__((aligned(x)))
#define PRINTF_LIKE(idx, p) __attribute__((format(printf, idx, p)))
#endif

#endif
