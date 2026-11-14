#ifndef NOTEMPTY_H
#define NOTEMPTY_H

typedef int notempty_int;

#ifndef _MSC_VER
__attribute__((unused)) static notempty_int not_empty(void);

#else
static notempty_int not_empty(void);
#endif

#endif
