#ifndef _BOARD_H
#define _BOARD_H

#include <stdbool.h>
#include <stdio.h>
#include <wchar.h>
#include <string.h>

#include "../common.h"
#include "../masks.h"
#include "move.h"
#include "hash.h"
#include "bitboard.h"

#define MAX_BOARD_STATES (1024)

typedef struct _board_state {
    unsigned int can_castle;
    unsigned int passed_square;
    unsigned int fifty_move_rule_counter;
    unsigned int material[2];
    key_t hash_key;
    move_t move;
} board_state_t;

typedef struct _board {
    bitboard_t piece_bitboard[NUMBER_OF_PIECES-2], color_bitboard[2], all_pieces_bitboard;
    // there can only be one king, and this circumvents having to get the first bit set of a king bitboard
    unsigned int king_position[2];

    unsigned char pieces[64];
    bool to_move; // WHITE or BLACK
    unsigned int can_castle;
    unsigned int passed_square; // set to the square a pawn hopped over on the last move via double move, or zero
    unsigned int fifty_move_rule_counter;
    unsigned int material[2];

    key_t hash_key; // hash of board position

    board_state_t state[MAX_BOARD_STATES];
    unsigned int state_index;
} board_t;

#define WHITE_CASTLE_OO (1)
#define WHITE_CASTLE_OOO (2)
#define BLACK_CASTLE_OO (4)
#define BLACK_CASTLE_OOO (8)

#define RESET_BOARD(b) set_board_from_fen_string(&b, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1")

void make_move(board_t* board, move_t move);
void undo_move(board_t* board, move_t move);
void set_board_from_fen_string(board_t* board, const char* fen_string);
void print_board(const board_t* board);
move_t move_from_text(const char* text, board_t* board);
unsigned int repetition_count(const board_t* board);

#include "../moves.h"
#endif
