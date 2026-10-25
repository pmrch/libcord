#include "resolve.h"
#include "log.h"
#include "utils.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/time.h>

#define DECODED_ANSWERS 10

static u8 *encode_hostname(const char *hostname, u64 *final_len) {
    const char *cur_dot  = NULL;
    const char *last_dot = NULL;

    int64_t all_diff = 0;

    // +1 for the prefix of the hostname and +1 for the null-terminator
    *final_len   = strlen(hostname) + 2;
    uint8_t *buf = (u8 *)calloc(1, *final_len);

    LOG_VERBOSE("Got hostname %s to encode, with length of %lu. Allocating a %lu byte buffer", hostname, *final_len - 2, *final_len);
    RET_ON_ERR(buf == NULL, NULL, "Failed to allocate %lu bytes of memory for encoded hostname", *final_len);

    // NOLINTBEGIN(altera-unroll-loops)
    buf[*final_len - 1] = '.';
    while ((cur_dot = strchr(hostname + all_diff, '.')) != NULL) {
        int64_t last_diff = (cur_dot - hostname); // Number of characters before the dot

        last_dot = cur_dot;
        all_diff += last_diff;

        memcpy(buf + 1, hostname, (size_t)last_diff); // +1 offset to skip the first prefix byte
        hostname += last_diff;

        if (buf[0] == 0) { buf[0] = (u8)last_diff; } // If we get the first dot, insert the first prefix
    }

    // NOLINTEND(altera-unroll-loops)

    // -2 because we skip prefix and null termination, and -1 for skipping the dot with the ++last_dot
    buf[1 + all_diff] = (u8)(*final_len - 2 - (u64)all_diff - 1);
    memcpy(buf + 1 + (all_diff + 1), ++last_dot, (size_t)(*final_len - 2 - (u64)all_diff - 1));

    // null-termination for end
    buf[*final_len - 1] = 0x00;
    return buf;
}

static void print_encoded(const u8 *encoded, u64 len) {
    // clang-format off
    char *buf = (char *)malloc(len);
    const u8 *ptr = encoded;
    u64 off  = 0;
    // clang-format on

    if (buf == NULL) { return; }

    // NOLINTNEXTLINE
    for (size_t i = 0; i < len; i++) {
        if (off + 4 > len) {
            char *buf_old = buf;

            buf = (char *)realloc(buf, (off << 1) - (off >> 1));
            if (buf == NULL) { buf = buf_old; }
        }

        int32_t res = snprintf(buf + off, len, "%02X ", ptr[i]);
        if (res >= 0) { off += (u64)res; }
    }

    LOG_INFO("Encoded hostname: %s", buf);
    // free(buf);
}

// NOLINTBEGIN (bugprone-easily-swappable-parameters, readability-magic-numbers)
static u16 pack_flags(u8 qr, u8 opcode, u8 aa, u8 tc, u8 rd, u8 ra, u8 z, u8 rcode) {
    u16 flags_total = (u16)((qr << 15) | (opcode << 11) | (aa << 10) | (tc << 9) | (rd << 8) | (ra << 7) | (z << 4) | (rcode));
    return flags_total;
}
// NOLINTEND (bugprone-easily-swappable-parameters, readability-magic-numbers)

static u8 *create_packet(int32_t sock, u8 *restrict encoded, u64 *packet_size, u64 encode_len, u64 *question_l) {
    // Allocate buffer
    u64 pk_size = sizeof(Header) + encode_len + sizeof(Question);
    u8 *packet  = (u8 *)malloc(pk_size);

    if (packet == NULL) {
        LOG_ERROR("Failed to allocate packet buffer of %lu bytes", pk_size);
        close(sock);
        free(encoded);
        return NULL;
    }

    u16 net_id = generate_random_u16();
    LOG_DEBUG("ID for query transaction: %02X", net_id);

    Header head  = {0};
    head.qdcount = htons(1);
    head.id      = htons(net_id);
    head.flags   = htons(pack_flags(0, 0, 0, 0, 1, 0, 0, 0));

    // Write the header to the packet
    memcpy(packet, &head, sizeof(Header));
    Question quest = {htons(1), htons(1)};

    memcpy(packet + sizeof(Header), encoded, encode_len);
    memcpy(packet + sizeof(Header) + encode_len, &quest, sizeof(Question));

    *packet_size = pk_size;
    *question_l  = sizeof(quest) + encode_len;
    return packet;
}

// NOLINTBEGIN(concurrency-mt-unsafe)
static u64 get_answer(int32_t sock, u8 *packet, u64 packet_size, saddr_in in_addr, u8 answer[UINT16_MAX]) {
    LOG_DEBUG("%s", "Sending DNS query message to nameserver...");
    int64_t sent = sendto(sock, packet, packet_size, 0, (saddr *)&in_addr, sizeof(in_addr));

    if (sent < 0) {
        LOG_ERROR("Failed to send message due to error: %s!", strerror(errno));
        goto ret_bad;
    }

    u64 addr_len = sizeof(in_addr);
    LOG_INFO("%s", "Successfully sent message to DNS nameserver");

    LOG_DEBUG("%s", "Receiving response from DNS nameserver...");
    int64_t recvd = recvfrom(sock, answer, UINT16_MAX, 0, (saddr *)&in_addr, (socklen_t *)&addr_len);

    if (recvd < 0) {
        LOG_ERROR("Failed to receive message due to error: %s!", strerror(errno));
        goto ret_bad;
    }

    LOG_INFO("%s", "Successfully received a response from the DNS nameserver");
    return (u64)recvd;
    // NOLINTEND(concurrency-mt-unsafe)

ret_bad:
    close(sock);
    free(packet);
    return UINT64_MAX;
}

static void decode_answers(Answer *answers, int64_t *num_decoded, u8 *answers_buf, u64 question_len) {
    if (answers == NULL || answers_buf == NULL) {
        LOG_ERROR("%s", "Input or output buffer for decoded answers was NULL!");
        return;
    }

    Header head = {0};
    memcpy(&head, answers_buf, sizeof(Header));

    // clang-format off
    LOG_DEBUG("{ id: %#x | flags: %#x | qdcount: %#x | ancount: %#x | arcount: %#x | nscount: %#x }",
        ntohs(head.id), ntohs(head.flags), ntohs(head.qdcount), ntohs(head.ancount), ntohs(head.arcount), ntohs(head.nscount)
    );

    *num_decoded = (int64_t)(ntohs(head.ancount));
    LOG_DEBUG("Question length was %lu", question_len);

    memcpy(&answers[0], answers_buf + sizeof(head) + question_len, sizeof(Answer)); // clanf-format off
    LOG_DEBUG("First answer: { name: %x | type: %x | class: %x | ttl: %x | rdlength: %x | rdata: %x }", 
        ntohs(answers[0].name), ntohs(answers[0].type), ntohs(answers[0].class), ntohl(answers[0].ttl), 
        ntohs(answers[0].rdlength), ntohl(answers[0].rdata)
    );

    
    u32 ipaddr = ntohl(answers[0].rdata);
    LOG_INFO("First answer IP: %u.%u.%u.%u", ipaddr >> 24 & 0xFF, ipaddr >> 16 & 0xFF, ipaddr >> 8 & 0xFF, ipaddr & 0xFF);
    // clang-format on
}

// NOLINTBEGIN(concurrency-mt-unsafe, altera-unroll-loops, bugprone-casting-through-void)
u32 *get_addr_ipv4(const char *hostname) {
    RET_ON_ERR(hostname == NULL, NULL, "%s", "Provided hostname was NULL!");
    saddr_in in_addr = {.sin_family = AF_INET, .sin_port = htons(DNS_PORT), .sin_addr.s_addr = htonl(DNS_SERVER)};

    int32_t sock = socket(AF_INET, SOCK_DGRAM | SOCK_CLOEXEC, IPPROTO_UDP);
    RET_ON_ERR((sock == -1), NULL, "Failed to open socket: %s", strerror(errno));

    // The number of bytes the hostname encode takes up
    u64 question_len = 0;
    u64 encode_len   = 0;
    u64 packet_size  = 0;

    u8 *encoded = encode_hostname(hostname, &encode_len);
    u8 *packet  = create_packet(sock, encoded, &packet_size, encode_len, &question_len);
    print_encoded(encoded, encode_len);
    free(encoded); // Only needed it to build the packet

    RET_ON_ERR(packet == NULL, NULL, "%s", "Failed to create packet!");
    u8 ansbuf[UINT16_MAX] = {0};

    // Receive the answer if possible
    u64 answer = get_answer(sock, packet, packet_size, in_addr, ansbuf);
    LOG_INFO("Received an answer of %lu bytes", answer);

    // Store retrieved IP addresses
    int64_t decoded_answers_length   = 0;
    Answer  decoded[DECODED_ANSWERS] = {0};

    decode_answers(decoded, &decoded_answers_length, ansbuf, question_len);
    LOG_DEBUG("Successfully decoded %li answers", decoded_answers_length);

    // Cleanup for the end
    close(sock);
    return 0;
}
// NOLINTEND(concurrency-mt-unsafe, altera-unroll-loops, bugprone-casting-through-void)
