#include "bitboard.h"

/*
    Prints a human-readable form of a bitboard. Empty squares are dots, and set bits are 1s.
*/
void print_bitboard(bitboard_t bitboard) {
    for (int file = 0; file < 8; file++) {
        printf("%d |", 8-file);
        for (int rank = 0; rank < 8; rank++) {
            printf(" %c",  ((bitboard<<(7-rank))&0x8000000000000000)?'1':'.');
        }
        bitboard <<= 8;
        printf("\n");
    }
    printf("   ----------------\n    a b c d e f g h\n");
}


/*
    Returns the index of the least significant bit set. Implementation of de Bruijn method.
*/
unsigned int first_bit_set(bitboard_t bitboard) {
   unsigned int folded;
   bitboard ^= bitboard - 1;
   folded = (int)bitboard ^ (bitboard >> 32);
   return DEBRUIJN_LSB_LOOKUP[folded * 0x78291ACF >> 26];
}


/*
    Counts number of bits of a sparse bitboard. Takes roughly (3*number of bits set) instructions.
    Algorithm is Peter Wegner's.
*/
unsigned int sparse_bit_count(bitboard_t bitboard) {
    unsigned int count;
    for (count = 0; bitboard; count++) {
        REMOVE_FIRST_BIT_SET(bitboard);
    }
    return count;
}


/*
    Counts number of bits of a bitboard. Takes roughly 24 instructions.
    Algorithm is MIT's HAKMEM.
*/
unsigned int bit_count(bitboard_t bitboard) {
    static const bitboard_t M1 = 0x5555555555555555;
    static const bitboard_t M2 = 0x3333333333333333;
    static const bitboard_t M4 = 0x0F0F0F0F0F0F0F0F;
    static const bitboard_t M8 = 0x00FF00FF00FF00FF;
    static const bitboard_t M16 = 0x0000FFFF0000FFFF;
    static const bitboard_t M32 = 0x00000000FFFFFFFF;

    bitboard = (bitboard & M1) + ((bitboard >> 1) & M1);
    bitboard = (bitboard & M2) + ((bitboard >> 2) & M2);
    bitboard = (bitboard & M4) + ((bitboard >> 4) & M4);
    bitboard = (bitboard & M8) + ((bitboard >> 8) & M8);
    bitboard = (bitboard & M16) + ((bitboard >> 16) & M16);
    bitboard = (bitboard & M32) + ((bitboard >> 32) & M32);
    return (unsigned int)bitboard;
}
