#include <stdio.h>
#include <string.h>

#include "evaluation/evaluate.h"
#include "structures/board.h"

#include "common.h"
#include "moves.h"
#include "search.h"

#define MAX_PLY (64)
#define INITIAL_ALPHA_BETA (100000)

// some variables for search
static unsigned int principal_variation_length[MAX_PLY];
static move_t principal_variation_moves[MAX_PLY][MAX_PLY];
static bool follow_principal_variation;
static unsigned int white_history_heuristic[64][64];
static unsigned int black_history_heuristic[64][64];

/*
    The entry point for search.
*/
move_t think(board_t* board) {
    unsigned int number_of_moves = count_legal_moves(board);

    if (number_of_moves == 0) {
        if (is_king_in_check(board, board->to_move)) {
            printf("Checkmate!\n");
            return 0;
        } else {
            printf("Stalemate!\n");
            return 0;
        }
    }

    if (is_theoretical_draw(board) || board->fifty_move_rule_counter >= 100 || repetition_count(board) >= 3) {
        printf("Draw!\n");
        return 0;
    }

    int score;
    int max_depth = 8; // MAX_PLY

    memset(white_history_heuristic, 0, sizeof(white_history_heuristic));
    memset(black_history_heuristic, 0, sizeof(black_history_heuristic));

    // TODO: limit by time
    for (int depth = 1; depth <= max_depth; depth++) {
        memset(principal_variation_length, 0, sizeof(principal_variation_length));
        memset(principal_variation_moves, 0, sizeof(principal_variation_moves));

        follow_principal_variation = true;
        score = search(board, -INITIAL_ALPHA_BETA, INITIAL_ALPHA_BETA, 0, depth);

        if ((score > (CHECKMATE_SCORE-depth)) || (score < -(CHECKMATE_SCORE-depth))) {
            depth = max_depth;
        }
    }

    return principal_variation_moves[0][0];
}


/*
    Search the board for a good move. Current implementation:
        NegaScout
        Alpha-beta pruning
        Principal variation search
*/
int search(board_t* board, int alpha, int beta, unsigned int ply, unsigned int depth) {
    move_t move_list[MAX_MOVES];
    int new_score, moves_found = 0, principal_variation_moves_found = 0;

    principal_variation_length[ply] = ply;

    if (depth == 0) {
        follow_principal_variation = false;
        return evaluate(board);
    }

    if (repetition_count(board) >= 3) {
        return DRAW_SCORE;
    }

    move_t* end_of_move_list = generate_pseudolegal_moves(board, move_list);

    for (int i = 0; &move_list[i] != end_of_move_list; i++) {
        order_moves(move_list, end_of_move_list, i, board->to_move, ply, depth);

        if (IS_CASTLE(move_list[i]) && !is_legal_castle(board, move_list[i])) {
            continue;
        }

        make_move(board, move_list[i]);

        if (is_king_in_check(board, !board->to_move)) {
            undo_move(board, move_list[i]);
        } else {
            moves_found++;

            if (principal_variation_moves_found) {
                new_score = -search(board, -alpha-1, -alpha, ply+1, depth-1);

                if ((new_score > alpha) && (new_score < beta)) {
                    new_score = -search(board, -beta, -alpha, ply+1, depth-1);
                }
            } else {
                new_score = -search(board, -beta, -alpha, ply+1, depth-1);
            }

            undo_move(board, move_list[i]);

            if (new_score >= beta) {
                if (board->to_move == WHITE) {
                    white_history_heuristic[MOVE_FROM(move_list[i])][MOVE_TO(move_list[i])] += depth*depth;
                } else {
                    black_history_heuristic[MOVE_FROM(move_list[i])][MOVE_TO(move_list[i])] += depth*depth;
                }

                return beta;
            }

            if (new_score > alpha) {
                alpha = new_score;
                principal_variation_moves_found++;
                principal_variation_moves[ply][ply] = move_list[i];

                for (int j = ply + 1; j < principal_variation_length[ply+1]; j++) {
                    principal_variation_moves[ply][j] = principal_variation_moves[ply+1][j];
                }

                principal_variation_length[ply] = principal_variation_length[ply+1];
            }
        }
    }

    if (principal_variation_moves_found) {
        if (board->to_move == WHITE) {
            white_history_heuristic[MOVE_FROM(principal_variation_moves[ply][ply])][MOVE_TO(principal_variation_moves[ply][ply])] += depth*depth;
        } else {
            black_history_heuristic[MOVE_FROM(principal_variation_moves[ply][ply])][MOVE_TO(principal_variation_moves[ply][ply])] += depth*depth;
        }
    }

    if (!moves_found) {
        if (is_king_in_check(board, board->to_move)) {
            return -CHECKMATE_SCORE+ply-1;
        } else {
            return DRAW_SCORE;
        }
    }

    if (board->fifty_move_rule_counter >= 100) {
        return DRAW_SCORE;
    }

    return alpha;
}


/*
    Performs a rough move ordering based on the history heuristic.
*/
void order_moves(move_t* move_list, move_t* end_of_move_list, int current_index, bool to_move, unsigned int ply, unsigned int depth) {
    #define swap(a,b) (((a) ^= (b)), ((b) ^= (a)), ((a) ^= (b)))

    #define history_heuristic(heuristic_array) \
        unsigned int best_heuristic = heuristic_array[MOVE_FROM(move_list[current_index])][MOVE_TO(move_list[current_index])]; \
        unsigned int best_index = current_index; \
        for (int i = current_index + 1; &move_list[i] != end_of_move_list; i++) { \
            unsigned int heuristic = heuristic_array[MOVE_FROM(move_list[i])][MOVE_TO(move_list[i])]; \
            if (heuristic > best_heuristic) { \
                best_heuristic = heuristic; \
                best_index = i; \
            } \
        } \
        if (best_index > current_index) { \
            swap(move_list[best_index], move_list[current_index]); \
        }

    if (follow_principal_variation && depth > 1) {
        for (int i = current_index; &move_list[i] != end_of_move_list; i++) {
            if (move_list[i] == principal_variation_moves[0][ply]) {
                swap(move_list[i], move_list[current_index]);
                return;
            }
        }
    }

    if (to_move == WHITE) {
        history_heuristic(white_history_heuristic);
    } else {
        history_heuristic(black_history_heuristic);
    }

    #undef swap
    #undef history_heuristic
}
