#ifndef RESOLVE_H
#define RESOLVE_H

#define DNS_SERVER 0x01010101 // 1.1.1.1
#define DNS_PORT 53

#include "platform.h"
#include "utils.h"

#include <netinet/in.h>
#include <sys/socket.h>

typedef struct sockaddr_in  saddr_in;
typedef struct sockaddr_in6 saddr_in6;
typedef struct sockaddr     saddr;

// NOLINTNEXTLINE
typedef struct {
    u16 id;
    u16 flags;
    u16 qdcount;
    u16 ancount;
    u16 nscount;
    u16 arcount;
} Header;

// NOLINTNEXTLINE
typedef struct PACKED {
    u16 name;
    u16 type;
    u16 class;
    u32 ttl;
    u16 rdlength;
    u32 rdata;
} Answer;

// NOLINTNEXTLINE
typedef struct {
    u16 qtype;
    u16 qclass;
} Question;

// Non-critical structs

typedef struct ALIGN(8) {
    u8 qr : 1;
    u8 opcode : 4;
    u8 aa : 1;
    u8 tc : 1;
    u8 rd : 1;
    u8 ra : 1;
    u8 z : 3;
    u8 rcode : 4;
} Flags;

typedef struct ALIGN(32) {
    u64 packet_size;
    u32 decoded_answers;
    u32 question_len;
    u32 encode_len;
} Lengths;

u32 *get_addr_ipv4(const char *hostname);

#endif
