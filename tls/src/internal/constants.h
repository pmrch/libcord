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

// TLS related entries
#define TLS1_0 0x0301
#define TLS1_2 0x0303
#define TLS1_3 0x0304

#define MASK_BYTE 0xFF
#define MASK_NIBBLE 0x0F

#define TYPE_HANDSHAKE 0x16
#define HANDSHAKE_MSG_TYPE 0x01
#define FAKE_SESSION_LEN 0x20

#define TLS_CHACHA20_POLY1305_SHA256 0x1303
#define TLS_COMPRESSION_METHODS_NULL 0x0100

#define TLS_EXT_EC_POINT_FORMATS 0x000b
#define TLS_EXT_SUPPORTED_GROUPS 0x000a
#define TLS_EXT_SESSION_TICKET 0x0023
#define TLS_EXT_ENCRYPT_THEN_MAC 0x0016
#define TLS_EXT_EXTENDED_MASTER_SECRET 0x0017
#define TLS_EXT_SIGNATURE_ALGORITHMS 0x000d
#define TLS_EXT_SUPPORTED_VERSIONS 0x002b
#define TLS_EXT_PSK_KEY_EXCHANGE_MODES 0x002d
#define TLS_EXT_KEY_SHARE 0x0033

#define TLS_EXT_CURVE_X25519 0x001d

#define TLS_EXT_SIG_ECDSA_SHA256 0x0403
#define TLS_EXT_SIG_RSA_PSS_SHA384 0x0805
#define TLS_EXT_SIG_RSA_PSS_SHA256 0x0804
#define TLS_EXT_SIG_ED25519 0x0807

#define TLS_KEY_SHARE_EXT_LEN 0x0026
#define TLS_KEY_SHARE_DATA_LEN 0x0024
#define TLS_KEY_SHARE_KEX_X25519 0x001d
#define TLS_KEY_SHARE_PUBKEY_LEN 0x0020

// Numbers
#define FOUR 0x04U
#define FIVE 0x05U
#define SIX 0x06U
#define SEVEN 0x07U
#define EIGHT 0x08U
#define NINE 0x09U
#define TEN 0x0aU

// SHA256 constants

#ifndef _MSC_VER
__attribute__((unused)) static void not_empty(void);
#endif

#endif
