#ifndef _STRATEGY_H
#define _STRATEGY_H

#include "constants.h"
#include "../common.h"
#include "../structures/board.h"
#include "../structures/bitboard.h"

int evaluate_pawn(unsigned int square, const board_t* board, unsigned int side, int gamestage);
int evaluate_knight(unsigned int square, const board_t* board, unsigned int side, int gamestage);
int evaluate_bishop(unsigned int square, const board_t* board, unsigned int side, int gamestage);
int evaluate_rook(unsigned int square, const board_t* board, unsigned int side, int gamestage);
int evaluate_queen(unsigned int square, const board_t* board, unsigned int side, int gamestage);
int evaluate_king(unsigned int square, const board_t* board, unsigned int side, int gamestage);
#endif
