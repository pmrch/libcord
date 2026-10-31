#ifndef PLATFORM_H
#define PLATFORM_H

#ifdef _WIN32
#include "win32.h"
#else
#include "posix.h"
#endif

void   start_timer(Timer *timer);
void   end_timer(Timer *timer);
double get_timer_ns(Timer *timer);

#endif
