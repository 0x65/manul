#ifndef _BITBOARD_H
#define _BITBOARD_H

#include <stdio.h>

typedef unsigned long long int bitboard_t;

#define REMOVE_FIRST_BIT_SET(x) ((x) = ((x)&((x)-1)))
#define FIRST_BIT_SET(x) (__builtin_ctzll(x))
#define BIT_COUNT(x) (__builtin_popcountll(x))

unsigned int sparse_bit_count(bitboard_t bitboard);
void print_bitboard(bitboard_t bitboard);
#endif
