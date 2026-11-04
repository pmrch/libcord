#include "tls.h"
#include "log.h"
#include "platform.h"
#include "types.h"
#include "utils.h"

#include <errno.h>

#define HEADER_LEN 5
#define LOCAL_APPEND_TO_BUF16(num) do {\
    u16_append((num), buffer, &offset); \
} while (0)

static inline void u16_append(u16 num, u8 *restrict buffer, u16 *restrict offset) {
    u16 local_off       = *offset;
    buffer[local_off++] = (num >> ONE_BYTE) & MASK_BYTE;
    buffer[local_off++] = num & MASK_BYTE;

    *offset = local_off;
}

u16 build_client_hello(u8 *restrict buffer, const char *hostname, u16 hname_len, const u8 *restrict pubkey) {
    u16 list_len_sname = hname_len + 3;
    u16 ext_len_sname  = hname_len + FIVE;

    u16 offset = 0;

    // Handshake Header
    buffer[offset++]  = HANDSHAKE_MSG_TYPE;
    u16 hs_len_offset = offset;
    offset += 3;

    // Client Version (TLS 1.2 for compatibility)
    buffer[offset++] = 0x03;
    buffer[offset++] = 0x03;

    // Client random 32 bytes
    generate_random_bytes(BITS32, buffer + offset);
    offset += BITS32;

    // Random fake session 33 bytes with len included
    // buffer[offset++] = FAKE_SESSION_LEN;
    // generate_random_bytes(BITS32, buffer + offset);
    // offset += BITS32;
    buffer[offset++] = 0x00;

    /// Cipher suites
    // 2 bytes of cipher data
    buffer[offset++] = 0x00;
    buffer[offset++] = 0x02;

    // TLS_CHACHA20_POLY1305_SHA256
    LOCAL_APPEND_TO_BUF16(TLS_CHACHA20_POLY1305_SHA256);

    // Compression methods
    buffer[offset++] = 0x01; // 1 bytes of compression methods
    buffer[offset++] = 0x00; // which is assigned with null

    // Extension Length
    u16 ext_len_offset = offset;
    offset += 2;

    /// Extensions
    // Extension - Server Name
    LOCAL_APPEND_TO_BUF16(0x0000);         // Value for server_name extension
    LOCAL_APPEND_TO_BUF16(ext_len_sname);  // 2 bytes of extension length
    LOCAL_APPEND_TO_BUF16(list_len_sname); // 2 bytes of list length

    buffer[offset++] = 0x00; // DNS list type
    LOCAL_APPEND_TO_BUF16(hname_len);

    memcpy(buffer + offset, hostname, hname_len); // Copy the hostname
    offset += hname_len;

    // Extension - EC Point Formats
    LOCAL_APPEND_TO_BUF16(TLS_EXT_EC_POINT_FORMATS);

    buffer[offset++] = 0x00; // 2 bytes of format types follow
    buffer[offset++] = 0x02;

    buffer[offset++] = 0x01; // 1 byte of format types follow
    buffer[offset++] = 0x00; // assigned value for format "uncompressed"

    // Extension - Supported Groups
    LOCAL_APPEND_TO_BUF16(TLS_EXT_SUPPORTED_GROUPS);

    buffer[offset++] = 0x00; // 0x0004 (4) bytes of "supported group" extension data follows
    buffer[offset++] = 0x04;

    buffer[offset++] = 0x00; // 0x0002 (2) bytes of list elements
    buffer[offset++] = 0x02;

    LOCAL_APPEND_TO_BUF16(TLS_EXT_CURVE_X25519);

    // Extension - Session Ticket
    /*buffer[offset++] = (TLS_EXT_SESSION_TICKET >> ONE_BYTE) & MASK_BYTE;
    buffer[offset++] = TLS_EXT_SESSION_TICKET & MASK_BYTE;
    buffer[offset++] = 0x00; // 0 bytes of "Session Ticket" extension data follows
    buffer[offset++] = 0x00;

    // Extension - Encrypt-Then-MAC
    buffer[offset++] = (TLS_EXT_ENCRYPT_THEN_MAC >> ONE_BYTE) & MASK_BYTE;
    buffer[offset++] = TLS_EXT_ENCRYPT_THEN_MAC & MASK_BYTE;
    buffer[offset++] = 0x00; // 0 bytes of "Encrypt Then MAC" extension data follows
    buffer[offset++] = 0x00;

    // Extension - Extended Master Secret
    buffer[offset++] = (TLS_EXT_EXTENDED_MASTER_SECRET >> ONE_BYTE) & MASK_BYTE;
    buffer[offset++] = TLS_EXT_EXTENDED_MASTER_SECRET & MASK_BYTE;
    buffer[offset++] = 0x00; // 0 bytes of "Extended Master Secret" extension data follows
    buffer[offset++] = 0x00;*/

    // Extension - Signature Algorithms
    LOCAL_APPEND_TO_BUF16(TLS_EXT_SIGNATURE_ALGORITHMS);

    buffer[offset++] = 0x00; // 10 bytes follow
    buffer[offset++] = TEN;
    buffer[offset++] = 0x00; // 8 bytes list follows
    buffer[offset++] = EIGHT;

    // 0x0804 RSA-PSS-SHA256, 0x0403 ECDSA-SHA256, 0x0807 Ed25519, and 0x0805 RSA-PSS-SHA384
    LOCAL_APPEND_TO_BUF16(TLS_EXT_SIG_ECDSA_SHA256); // The list begins
    LOCAL_APPEND_TO_BUF16(TLS_EXT_SIG_RSA_PSS_SHA256);
    LOCAL_APPEND_TO_BUF16(TLS_EXT_SIG_RSA_PSS_SHA384);
    LOCAL_APPEND_TO_BUF16(TLS_EXT_SIG_ED25519); // list end

    // Extension - Supported Versions
    LOCAL_APPEND_TO_BUF16(TLS_EXT_SUPPORTED_VERSIONS);

    buffer[offset++] = 0x00; // 3 bytes follow: 1 for length , 2 for each version
    buffer[offset++] = 0x03;
    buffer[offset++] = 0x02; // 2 bytes for versions

    /*buffer[offset++] = (TLS1_2 >> ONE_BYTE) & MASK_BYTE;
    buffer[offset++] = TLS1_2 & MASK_BYTE;*/
    LOCAL_APPEND_TO_BUF16(TLS1_3);

    // Extension - PSK Key Exchange Modes
    /*buffer[offset++] = (TLS_EXT_PSK_KEY_EXCHANGE_MODES >> ONE_BYTE) & MASK_BYTE;
    buffer[offset++] = TLS_EXT_PSK_KEY_EXCHANGE_MODES & MASK_BYTE;

    buffer[offset++] = 0x00; // 2 Bytes of data flow in
    buffer[offset++] = 0x02;
    buffer[offset++] = 0x01; // 1 byte of PSK Exchange Modes
    buffer[offset++] = 0x01; // assigned value for "PSK with (EC)DHE key establishment"*/

    // Extension - Key Share
    LOCAL_APPEND_TO_BUF16(TLS_EXT_KEY_SHARE);
    LOCAL_APPEND_TO_BUF16(TLS_KEY_SHARE_EXT_LEN);
    LOCAL_APPEND_TO_BUF16(TLS_KEY_SHARE_DATA_LEN);
    LOCAL_APPEND_TO_BUF16(TLS_KEY_SHARE_KEX_X25519);
    LOCAL_APPEND_TO_BUF16(TLS_KEY_SHARE_PUBKEY_LEN);

    memcpy(buffer + offset, pubkey, BITS32);
    offset += BITS32;
    /// Extensions end

    LOG_DEBUG("ext_len_offset: %u | hs_len_offset: %u | offset: %u", ext_len_offset, hs_len_offset, offset);

    // Backfill extension length
    u16 ext_len                = (u16)(offset - ext_len_offset - 2);
    buffer[ext_len_offset]     = (ext_len >> ONE_BYTE) & MASK_BYTE;
    buffer[ext_len_offset + 1] = ext_len & MASK_BYTE;

    // Backfill Handshake Length
    u16 hs_len = offset - 4;

    buffer[hs_len_offset]     = (hs_len >> (ONE_BYTE * 2)) & MASK_BYTE;
    buffer[hs_len_offset + 1] = (hs_len >> ONE_BYTE) & MASK_BYTE;
    buffer[hs_len_offset + 2] = hs_len & MASK_BYTE;

    return offset;
}

// int64_t tls_record_recv(sock_type_t sock_fd, u8 *restrict type_out, const u8 *restrict buf, u16 bufsize) { return 0; }

int64_t tls_record_send(sock_type_t sock_fd, const u8 TYPE, const u8 *payload, u16 len) {
    TlsHeader header;
    header.type  = TYPE;
    header.proto = htons(TLS1_2);
    header.size  = htons(len);
    // header.size = ((len >> ONE_BYTE) & MASK16_high) | (len & MASK16_high);

    socklen_t sent = 0;

    packet_t header_ptr = (packet_t)&header;
    LOG_VERBOSE("TLS ClientHello header: type=0x%02x, proto=0x%04x, len=0x%04x", header.type, ntohs(header.proto), ntohs(header.size));

    while (sent < HEADER_LEN) {
        int64_t sss = send(sock_fd, header_ptr + sent, HEADER_LEN - sent, 0);
        if (sss < 0 && errno == EINTR) { continue; }
        if (sss < 0) { return -1; }
        sent += (socklen_t)sss;
    }

    LOG_DEBUG("Successfully sent %u bytes header", sent);
    packet_t ptr = (packet_t)payload;

    sent = 0;
    while (sent < len) {
        int64_t sss = send(sock_fd, ptr + sent, len - sent, 0);
        if (sss < 0 && errno == EINTR) { continue; }
        if (sss < 0) { return -1; }
        sent += (socklen_t)sss;
    }

    return (int64_t)sent;
}
