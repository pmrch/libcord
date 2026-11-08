#include "resolve.h"
#include "abstraction.h"
#include "constants.h"
#include "log.h"
#include "platform.h"
#include "utils.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_DOMAIN_LEN 254

// NOLINTNEXTLINE readability-identifier-length
static inline u16 pack_flags(u8 qr, u8 opcode, u8 aa, u8 tc, u8 rd, u8 ra, u8 z, u8 rcode) {
    u16 flags_total = (u16)((qr << QR) | (opcode << OPCODE) | (aa << AA) | (tc << TC) | (rd << RD) | (ra << RA) | (z << Z) | rcode);
    return flags_total;
}

static inline void encode_hostname(const char *hostname, u32 *final_len, u8 *restrict buf) {
    if (!hostname || !final_len || !buf || *hostname == '\0') {
        LOG_ERROR("Invalid arguments. Hostname: %p, len: %p, buf: %p", (const void *)hostname, (void *)final_len, (const void *)buf);
        return; // Error: invalid arguments
    }

    *final_len = (u32)(strlen(hostname) + 2); // One byte before and one after the actual string
    if (*final_len == UINT32_MAX || *final_len > MAX_DOMAIN_LEN) {
        LOG_ERROR("Too long domain name provided: %s", hostname);
        return;
    }

    LOG_VERBOSE("Got hostname %s to encode, with length of %u.", hostname, *final_len - 2);
    memcpy(buf + 1, hostname, *final_len - 2); // buf + 1 to skip the very first byte

    u8 *last_dot  = NULL;
    u8 *buf_ptr   = buf + 1;
    u8  num_chars = 0;

    buf[*final_len - 1] = '.';
    while (*buf_ptr != '\0') {
        if (*buf_ptr != '.') {
            buf_ptr++;
            num_chars++;
            continue;
        }

        if (last_dot != NULL) {
            *last_dot = num_chars;
        } else {
            buf[0]   = num_chars;
            last_dot = buf_ptr;
        }

        num_chars = 0;
        buf_ptr++;
    }

    buf[*final_len - 1] = 0x00;
}

static inline void print_encoded(const u8 *encoded, u64 len) {
    // clang-format off
    char buf[(BITS32 << 1) - (BITS32 >> 1)] = {0};

    const u8 *ptr = encoded;
    u64 off  = 0;
    // clang-format on

    for (size_t i = 0; i < len; i++) {
        i32 res = snprintf(buf + off, sizeof(buf), "%02X ", ptr[i]);
        if (res >= 0) { off += (u64)res; }
    }

    buf[off - 1] = '\0';
    LOG_DEBUG("Encoded hostname: %s", buf);
}

static inline void create_packet(u8 *restrict encoded, Lengths *lens, u8 *restrict packet) {
    // Allocate buffer
    // clang-format off
    u64 pk_size = sizeof(Header) + lens->encode_len + sizeof(Question);
    lens->packet_size = (socklen_t)pk_size;

    u16 net_id = generate_random_u16();
    LOG_DEBUG("ID for query transaction: %02X", net_id);
    // clang-format on

    Header head  = {0};
    head.qdcount = htons(1);
    head.id      = htons(net_id);
    head.flags   = htons(pack_flags(0, 0, 0, 0, 1, 0, 0, 0));

    // Write the header to the packet
    memcpy(packet, &head, sizeof(Header));
    Question quest = {htons(1), htons(1)};
    LOG_VERBOSE("Question: type=%02x, class=%02x", ntohs(quest.qtype), ntohs(quest.qclass));

    memcpy(packet + sizeof(Header), encoded, lens->encode_len);
    memcpy(packet + sizeof(Header) + lens->encode_len, &quest, sizeof(Question));

    lens->question_len = sizeof(quest) + lens->encode_len;
}

// NOLINTBEGIN(concurrency-mt-unsafe)
static u64 get_answer(sock_type_t sock, packet_t restrict packet, socklen_t packet_size, saddr_in in_addr, answer_t restrict answer) {
    LOG_DEBUG("%s", "Sending DNS query message to nameserver...");
    i64 sent = sendto(sock, packet, packet_size, 0, (saddr *)&in_addr, sizeof(in_addr));

    if (sent < 0) {
        LOG_ERROR("Failed to send message due to error: %s!", get_error_string(errno));
        return UINT64_MAX;
    }

    LOG_DEBUG("%s", "Successfully sent message to DNS nameserver");
    socklen_t addr_len = sizeof(in_addr);

    LOG_DEBUG("%s", "Receiving response from DNS nameserver...");
    i64 recvd = SOCK_RECVFROM(sock, answer, UINT16_MAX, 0, (saddr *)&in_addr, &addr_len);

    if (SOCK_RECV_FAIL(recvd)) {
        LOG_ERROR("Failed to receive message due to error: %d!", GET_ERROR_CODE);
        return UINT64_MAX;
    }

    LOG_DEBUG("%s", "Successfully received a response from the DNS nameserver");
    return (u64)recvd;
}
// NOLINTEND(concurrency-mt-unsafe)

static inline void decode_answers(Answer *restrict answers, u8 *restrict answers_buf, Lengths *restrict lens, u32 *restrict output) {
    if (answers == NULL || answers_buf == NULL) {
        LOG_ERROR("%s", "Input or output buffer for decoded answers was NULL!");
        return;
    }

    Header head = {0};
    memcpy(&head, answers_buf, sizeof(Header));

    // clang-format off
    LOG_VERBOSE("Answer header: id=0x%04x | flags=0x%04x | qdcount=0x%02x | ancount=0x%02x | arcount=0x%02x | nscount=0x%02x",
        ntohs(head.id), ntohs(head.flags), ntohs(head.qdcount), ntohs(head.ancount), ntohs(head.arcount), ntohs(head.nscount)
    ); // clang-format on

    lens->decoded_answers = ntohs(head.ancount);
    LOG_DEBUG("Question length was %u", lens->question_len);

    // Answer offset
    u64 offset = 0;

    u8 addr_buf[BITS16] = {0};
    for (i32 i = 0; i < MIN(ntohs(head.ancount), DECODED_ANSWERS); i++) {
        Answer *cur_ans = &answers[i];
        memcpy(cur_ans, answers_buf + sizeof(head) + lens->question_len + offset, sizeof(Answer));

        u32 ipv4_word = ntohl(cur_ans->rdata);
        get_formatted_ipv4(ipv4_word, addr_buf);
        LOG_DEBUG("Answer %d IP: %s", i, addr_buf);

        offset += sizeof(Answer);
        output[i] = ipv4_word;
    }

    LOG_DEBUG("Successfully decoded %u answers", lens->decoded_answers);
}

// NOLINTBEGIN(concurrency-mt-unsafe, bugprone-casting-through-void)
void get_addr_ipv4(const char *hostname, u32 *restrict results, u8 *restrict num_results) {
    if (hostname == NULL) {
        LOG_ERROR("%s", "Provided hostname was NULL!");
        return;
    }

    LOG_DEBUG("hostname: %s | results: %p | num_results: %p", hostname, (void *)results, (void *)num_results);
    saddr_in in_addr = {.sin_family = AF_INET, .sin_port = htons(DNS_PORT), .sin_addr.s_addr = htonl(DNS_SERVER)};

    // NOLINTNEXTLINE (android-cloexec-socket)
    sock_type_t sock_fd = socket(AF_INET, RSOCK_TYPE, IPPROTO_UDP);

    if (SOCK_CREATE_FAIL(sock_fd)) {
        LOG_ERROR("Failed to open socket: %s | %li", get_error_string(errno), (long)sock_fd);
        return;
    }

    u8 success;
    SOCKOPT_TIMEOUT_RECV(sock_fd, EIGHT, success);
    if (!success) {
        sock_close(sock_fd);
        LOG_ERROR("%s", "setsockopt failed\n");
        return;
    }

    // The number of bytes the hostname encode takes up
    // clang-format off
    Lengths lens = {0};
    u8 packet[BITS64] = {0};
    // clang-format on

    u8 encoded[BITS32] = {0};
    encode_hostname(hostname, &lens.encode_len, encoded);

    create_packet(encoded, &lens, packet);
    print_encoded(encoded, lens.encode_len);
    u8 ansbuf[UINT16_MAX] = {0};

    // Receive the answer if possible
    u64 answer = get_answer(sock_fd, (packet_t)packet, lens.packet_size, in_addr, (answer_t)ansbuf);
    LOG_DEBUG("Received an answer of %lu bytes", answer);
    sock_close(sock_fd);

    if (answer == UINT64_MAX) { return; }

    // Store retrieved IP addresses
    Answer decoded[DECODED_ANSWERS] = {0};
    decode_answers(decoded, ansbuf, &lens, results);
    *num_results = (u8)lens.decoded_answers;
}

// NOLINTEND(concurrency-mt-unsafe, bugprone-casting-through-void)
