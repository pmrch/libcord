#ifndef RESOLVE_H
#define RESOLVE_H

#include "types.h"

// NOLINTNEXTLINE
typedef struct {
    u16 id;
    u16 flags;
    u16 qdcount;
    u16 ancount;
    u16 nscount;
    u16 arcount;
} Header;

// NOLINTBEGIN
#pragma pack(push, 1)
typedef struct {
    u16 name;
    u16 type;
    u16 class;
    u32 ttl;
    u16 rdlength;
    u32 rdata;
} Answer;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct {
    u16 qtype;
    u16 qclass;
} Question;
#pragma pack(pop)
// NOLINTEND

// Non-critical structs

typedef struct {
    u8 qr : 1;
    u8 opcode : 4;
    u8 aa : 1;
    u8 tc : 1;
    u8 rd : 1;
    u8 ra : 1;
    u8 z : 3;
    u8 rcode : 4;
} Flags;

typedef struct sockaddr_in  saddr_in;
typedef struct sockaddr_in6 saddr_in6;
typedef struct sockaddr     saddr;

void get_addr_ipv4(const char *hostname, u32 *restrict results, u8 *restrict num_results);

#ifdef _MSC_VER
#pragma comment(lib, "ws2_32.lib")
#include <WinSock2.h>

typedef SOCKET sock_type_t;

#define sock_close(fd) do {\
    closesocket(sock_fd); \
    WSACleanup(); \
} while (0)

#define RSOCK_TYPE SOCK_DGRAM
typedef int socklen_t;

#else
#define RSOCK_TYPE (SOCK_DGRAM | SOCK_CLOEXEC)
typedef int32_t sock_type_t;
typedef __socklen_t socklen_t;

#define sock_close(fd) do {\
    close(sock_fd); \
} while (0)

#endif

typedef struct {
    socklen_t packet_size;
    u32 decoded_answers;
    u32 question_len;
    u32 encode_len;
} Lengths;

#endif
