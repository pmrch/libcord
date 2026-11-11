#include "http.h"
#include "constants.h"
#include "crypt.h"
#include "log.h"
#include "platform.h"
#include "resolve.h"
#include "sha256.h"
#include "tls.h"
#include "utils.h"

#define HTTPS_PORT 443
#define CIPHER_KEY 0x13

static u16 get_cipher(const u8 *restrict buf, u16 offset_total) {
    for (u16 i = 0; i < offset_total; i++) {
        u8 val = buf[i];
        if (val == CIPHER_KEY) {
            LOG_DEBUG("Got it: 0x%02x", val);
            if (i < (offset_total - 1) && (buf[i + 1] >= 0x01 && buf[i + 1] <= 0x03)) {
                u16 cipher = (u16)(((u16)val << ONE_BYTE) | (u16)buf[i + 1]);
                LOG_DEBUG("Match found: 0x%04x", cipher);
                return cipher;
            }
        }
    }

    return 0;
}

static saddr_in create_sockaddr(const char *const HOSTNAME) {
    u32 addresses[DECODED_ANSWERS] = {0};
    u8  num_decoded;

    LOG_DEBUG("Got hostname: %s", HOSTNAME);
    get_addr_ipv4(HOSTNAME, addresses, &num_decoded);
    u32 main_addr = addresses[0];

    saddr_in addr = {.sin_addr.s_addr = htonl(main_addr), .sin_port = htons(HTTPS_PORT), .sin_family = AF_INET};
    return addr;
}

static i8 tcp_connect(sock_type_t sock_fd, saddr_in addr, socklen_t len) {
    if (SOCK_CONN_FAIL(connect(sock_fd, (saddr *)&addr, len))) {
        char ipv[BITS16] = {0};
        get_formatted_ipv4(ntohl(addr.sin_addr.s_addr), (u8 *)ipv);

        LOG_ERROR("Failed to connect to server: %s, error code: %d, fd: %li. Closing socket", ipv, GET_ERROR_CODE, (long)sock_fd);
        sock_close(sock_fd);
        return -1;
    }

    return 0;
}

static i8 decode_server_pubkey(const u8 *restrict buf, u16 resp_len, u8 *restrict pubkey_out) {
    if (buf == NULL || pubkey_out == NULL) {
        LOG_ERROR("%s", "Cannot decode server pubkey, in or output buffer was NULL");
        return -1;
    }

    u8 first = 0;
    u8 other = 0;

    u16 offset = 0;
    for (u16 i = 0; i < resp_len; i++, offset++) {
        other = buf[i];
        if (i != 0x00 && other == TLS_EXT_KEY_SHARE && buf[i - 1] == 0x00) { first = buf[i - 1]; }

        if (((first << ONE_BYTE) | other) == TLS_EXT_KEY_SHARE) {
            offset--;
            break;
        }
    }

    offset += EIGHT; // Skip the extension type, length, name, kex len
    for (u8 i = 0; i < (1 << FIVE); i++) { pubkey_out[i] = buf[offset + 1]; }
    return 0;
}

void connect_to_server(const char *hostname, const u8 *client_hello, u16 client_hello_len, u8 *restrict my_private_key) {
    char hexbuf[BITS64 << 3] = {0};

    saddr_in  addr = create_sockaddr(hostname);
    socklen_t len  = sizeof(addr);

    // NOLINTNEXTLINE (android-cloexec-socket)
    sock_type_t sock_fd = socket(AF_INET, RSOCK_STREAM_TYPE, 0);
    if (SOCK_CREATE_FAIL(sock_fd)) {
        LOG_ERROR("Failed to open socket to %s | %d", hostname, GET_ERROR_CODE);
        return;
    }

    sha256_ctx sha256 = {0};
    if (tcp_connect(sock_fd, addr, len)) { return; }

    sha256_init(&sha256);
    sha256_update(&sha256, client_hello, client_hello_len);
    u8 received[(1 << (TEN + 1)) >> 2] = {0};

    tls_record_send(sock_fd, TYPE_HANDSHAKE, client_hello, client_hello_len);
    i64 srvhello_len = tls_record_recv(sock_fd, received, sizeof(received));
    if (srvhello_len < 0) { return; }

    u8   shared_secret[BITS32] = {0};
    u8   server_pubkey[BITS32] = {0};
    fe51 shared_secret_field   = {0};

    decode_server_pubkey(received, (u16)srvhello_len, server_pubkey);
    curve25519(my_private_key, server_pubkey, shared_secret_field);
    encode_u_coordinate(shared_secret, shared_secret_field);

    buf_to_hex_upper(shared_secret, sizeof(shared_secret), hexbuf, sizeof(hexbuf));
    LOG_DEBUG("Shared secret: %s", hexbuf);

    u8  hello_hash[BITS32];
    u16 cipher = get_cipher(received, (u16)srvhello_len);
    if (cipher == TLS_CHACHA20_POLY1305_SHA256) {
        LOG_INFO("Received requested cipher: 0x%04x", (u16)TLS_CHACHA20_POLY1305_SHA256);
        sha256_update(&sha256, received + FIVE, (u64)srvhello_len - FIVE);
    }

    sha256_final(&sha256, hello_hash);
    memset(hexbuf, 0, sizeof(hexbuf));

    buf_to_hex_ns(hello_hash, sizeof(hello_hash), hexbuf, (u64)srvhello_len);
    LOG_INFO("Hello hash was created: %s", hexbuf);
}
