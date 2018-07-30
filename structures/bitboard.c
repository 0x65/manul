#include <stdio.h>

#include "bitboard.h"

void print_bitboard(bitboard_t bitboard) {
    for (int file = 0; file < 8; file++) {
        printf("%d |", 8-file);
        for (int rank = 0; rank < 8; rank++) {
            printf(" %c",  ((bitboard<<(7-rank))&0x8000000000000000) ? '1' : '.');
        }
        bitboard <<= 8;
        printf("\n");
    }
    printf("   ----------------\n    a b c d e f g h\n");
}


/*
    Counts number of bits of a sparse bitboard. For a small amount of bits set (roughly up to 4),
    works faster than BIT_COUNT. Algorithm is Peter Wegner's.
*/
unsigned int sparse_bit_count(bitboard_t bitboard) {
    unsigned int count;
    for (count = 0; bitboard; count++) {
        REMOVE_FIRST_BIT_SET(bitboard);
    }
    return count;
}
