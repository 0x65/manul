#ifndef _MOVE_H
#define _MOVE_H

#include <stdint.h>

/*
    Moves are represented by a 32-bit integer whose bits are separated into the
    following fields:

        Bits 0-5:   From square
        Bits 6-11:  To square
        Bits 12-15: Moving piece
        Bits 16-19: Captured piece
        Bit 20:     En-passant capture
        Bit 21:     Castle
        Bit 22:     Pawn promotion
        Bit 23-26:  Promotion piece
*/

typedef uint32_t move_t;

#define MOVE_FROM(m) (m&0x3F)
#define MOVE_TO(m) ((m>>6)&0x3F)
#define MOVING_PIECE(m) ((m>>12)&0xF)
#define CAPTURED_PIECE(m) ((m>>16)&0xF)
#define PROMOTION_PIECE(m) ((m>>23)&0xF)
#define IS_ENPASSANT(m) (m&0x100000)
#define IS_CASTLE(m) (m&0x200000)
#define IS_PROMOTION(m) (m&0x400000)

void print_move(move_t move);
unsigned char piece_type_from_char(char piece_char);
#endif
