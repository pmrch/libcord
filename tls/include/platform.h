#ifndef PLATFORM_H
#define PLATFORM_H

#ifdef _WIN32
#define PACKED __declspec(packed)
#else
#define PACKED __attribute__((packed))
#endif

#endif
