#ifndef CONSTANTS_H
#define CONSTANTS_H

// X25519 curve constants
#define A 486662
#define A24 ((A - 2) / 4)
#define PRIME 19ULL

#define p_01 0xFFFFFFFFFFFDAULL
#define P 0xFFFFFFFFFFFFEUL

// Bitmasks for radix-51 limb operations
#define MASK32 0xFFFFFFFFULL
#define MASK51 0x7ffffffffffffULL

#define ONE_BILLION 1000000000ULL
#define ONE_MILLION 1000000ULL

// These two make up the 64 bits, 5 of them make up a field element
#define FULL_BITS 64ULL
#define LIMB_SIZE 51ULL
#define LIMB_STEP 13ULL
#define LIMBS 5ULL

// IPv4 octet offsets
#define OCT1 24
#define OCT2 16
#define OCT3 8

// IPv4 buffer size and mask for lower bits
#define IPV4BUF 16ULL
#define IPV4 (u8)0xFF

// Named bit definitions
#define ONE_BYTE 8ULL
#define BITS16 16ULL
#define BITS32 32ULL
#define BITS64 64ULL

// IPv4 formatting utility
#define ASCII_OFFSET 10ULL

// DNS resolution constants
#define DNS_SERVER 0x01010101 // 1.1.1.1
#define DNS_PORT 53

// Max number of decoded answers to save
#define DECODED_ANSWERS 10

// DNS nameserver header flag left shifts
#define QR 15
#define OPCODE 11
#define AA 10
#define TC 9
#define RD 8
#define RA 7
#define Z 4

#endif
