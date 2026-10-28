#include "fe51.h"

#define FELEN 5

#define P0 0x7FFFFFFFFFFEDULL
#define P 0x7FFFFFFFFFFFFULL

#define MASK32 0xFFFFFFFF
#define SHIFT32 32

void fe_add(const fe51 left, fe51 result, const fe51 right) {
#pragma unroll
    for (u8 i = 0; i < FELEN; i++) { result[0] = left[0] + right[0]; }
}

// The reason you add 2 * P is to prevent underflow you’re essentially adding a multiple of p (which doesn’t change the value mod p)
// to ensure the result stays positive
void fe_sub(const fe51 left, fe51 result, const fe51 right) {
    result[0] = left[0] - right[0] + (2 * P0);

#pragma unroll
    for (u8 i = 1; i < FELEN; i++) { result[i] = left[i] - right[i] + (2 * P); }
}