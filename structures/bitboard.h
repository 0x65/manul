#ifndef _BITBOARD_H
#define _BITBOARD_H

#include <stdio.h>

typedef unsigned long long int bitboard_t;

static const int DEBRUIJN_LSB_LOOKUP[64] =
{
   63, 30,  3, 32, 59, 14, 11, 33, 60, 24, 50,  9, 55, 19, 21, 34,
   61, 29,  2, 53, 51, 23, 41, 18, 56, 28,  1, 43, 46, 27,  0, 35,
   62, 31, 58,  4,  5, 49, 54,  6, 15, 52, 12, 40,  7, 42, 45, 16,
   25, 57, 48, 13, 10, 39,  8, 44, 20, 47, 38, 22, 17, 37, 36, 26
};

#define REMOVE_FIRST_BIT_SET(x) ((x) = ((x)&((x)-1)))

unsigned int first_bit_set(bitboard_t bitboard);
unsigned int sparse_bit_count(bitboard_t bitboard);
unsigned int bit_count(bitboard_t bitboard);

void print_bitboard(bitboard_t bitboard);
#endif
