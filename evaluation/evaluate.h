#ifndef _EVALUATE_H
#define _EVALUATE_H

#include "../structures/board.h"

int evaluate(const board_t* board);

int evaluate_pawn(unsigned int square, const board_t* board, unsigned int side, int gamestage);
int evaluate_knight(unsigned int square, const board_t* board, unsigned int side, int gamestage);
int evaluate_bishop(unsigned int square, const board_t* board, unsigned int side, int gamestage);
int evaluate_rook(unsigned int square, const board_t* board, unsigned int side, int gamestage);
int evaluate_queen(unsigned int square, const board_t* board, unsigned int side, int gamestage);
int evaluate_king(unsigned int square, const board_t* board, unsigned int side, int gamestage);
#endif
