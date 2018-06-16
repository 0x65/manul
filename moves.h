#ifndef _MOVES_H
#define _MOVES_H

#include <stdbool.h>

#include "structures/board.h"
#include "structures/bitboard.h"
#include "structures/move.h"
#include "common.h"
#include "masks.h"

#define MAX_MOVES (256)

move_t* generate_pseudolegal_moves(const board_t* board, move_t* move_list);
char is_king_in_check(const board_t* board, unsigned int side);
bool is_theoretical_draw(const board_t* board);
bool is_legal_castle(board_t* board, move_t castle_move);
unsigned int count_legal_moves(board_t* board);
#endif
