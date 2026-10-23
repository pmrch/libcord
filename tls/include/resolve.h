#ifndef RESOLVE_H
#define RESOLVE_H

#define DNS_SERVER "1.1.1.1"

#include "platform.h"
#include <stdint.h>

// NOLINTNEXTLINE
typedef struct PACKED {
    uint16_t id;
    uint16_t flags;
    uint16_t qdcount;
    uint16_t ancount;
    uint16_t nscount;
    uint16_t arcount;
} Header;

// NOLINTNEXTLINE
typedef struct PACKED {
    uint8_t qr : 1;
    uint8_t opcode : 4;
    uint8_t aa : 1;
    uint8_t tc : 1;
    uint8_t rd : 1;
    uint8_t ra : 1;
    uint8_t z : 3;
    uint8_t rcode : 4;
} Flags;

#endif
