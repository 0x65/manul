#ifndef _SEARCH_H
#define _SEARCH_H

#include <stdbool.h>

#include "structures/board.h"
#include "structures/move.h"

move_t think(board_t* board);
int search(board_t* board, int alpha, int beta, unsigned int ply, unsigned int depth);
void order_moves(move_t* move_list, move_t* end_of_move_list, int current_index, bool to_move, unsigned int ply, unsigned int depth);
#endif
