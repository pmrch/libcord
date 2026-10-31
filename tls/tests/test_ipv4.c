#include "utils.h"
#include <stdio.h>
#include <string.h>

#define CLR_GREEN "\033[32m"
#define CLR_RED   "\033[31m"
#define CLR_RESET "\033[0m"

typedef struct {
    u32         in;
    const char *expected;
} Cases;

// NOLINTBEGIN (readability-magic-numbers)
int main(void) {
    int fails = 0;

    Cases cases[] = {
        {0x00000000, "0.0.0.0"},       {0x01000000, "1.0.0.0"},         {0x00000001, "0.0.0.1"},  {0x7F000001, "127.0.0.1"},
        {0xC0A80001, "192.168.0.1"},   {0xFFFFFFFF, "255.255.255.255"}, {0x0A000001, "10.0.0.1"}, {0x64000000, "100.0.0.0"},
        {0x01010101, "1.1.1.1"},       {0x00000100, "0.0.1.0"},         {0x00010000, "0.1.0.0"},  {0xFFFFFF00, "255.255.255.0"},
        {0x00FFFFFF, "0.255.255.255"}, {0xFF000000, "255.0.0.0"},
    };

    for (size_t i = 0; i < sizeof(cases) / sizeof(cases[0]); i++) {
        u8 buf[BITS16] = {0xAA};
        get_formatted_ipv4(cases[i].in, buf);

        if (strcmp((char *)buf, cases[i].expected) != 0) {
            fails++;

            printf("%sFAIL in=0x%08X expected=\"%s\" got=\"%s\"%s\n", CLR_RED, cases[i].in, cases[i].expected, buf, CLR_RESET);
            continue;
        }

        printf("%sSUCCESS: in=0x%08X => out=%s%s\n", CLR_GREEN, cases[i].in, buf, CLR_RESET);
    }

    printf("test_ipv4: %zu cases, %d failures\n", sizeof(cases) / sizeof(cases[0]), fails);
    return fails != 0;
}
// NOLINTEND (readability-magic-numbers)
