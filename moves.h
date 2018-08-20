#ifndef _MOVES_H
#define _MOVES_H

#include <stdbool.h>

#include "structures/board.h"
#include "structures/move.h"

#define MAX_MOVES (256)

move_t* generate_pseudolegal_moves(const board_t* board, move_t* move_list);
char is_king_in_check(const board_t* board, unsigned int side);
bool is_legal_castle(board_t* board, move_t castle_move);
unsigned int count_legal_moves(board_t* board);
#endif
