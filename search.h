#ifndef _SEARCH_H
#define _SEARCH_H

#include <stdbool.h>

#include "common.h"
#include "moves.h"
#include "evaluation/evaluate.h"
#include "structures/board.h"
#include "structures/move.h"

#define MAX_PLY (64)
#define INITIAL_ALPHA_BETA (100000)

// some variables for search
static unsigned int principal_variation_length[MAX_PLY];
static move_t principal_variation_moves[MAX_PLY][MAX_PLY];
static bool follow_principal_variation;
static unsigned int white_history_heuristic[64][64];
static unsigned int black_history_heuristic[64][64];

move_t think(board_t* board);
int search(board_t* board, int alpha, int beta, unsigned int ply, unsigned int depth);
void order_moves(move_t* move_list, move_t* end_of_move_list, int current_index, bool to_move, unsigned int ply, unsigned int depth);
#endif
