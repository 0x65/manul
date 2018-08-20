#include <stdio.h>

#include "../structures/board.h"
#include "../structures/hash.h"
#include "../structures/move.h"
#include "../common.h"
#include "../masks.h"
#include "../moves.h"

#include "test.h"
#include "test_hash.h"

/*
    Performs a very simple test of the hashing mechanism. In the first test, moves a knight around the board
    and back to its starting position to test if the hashes are the same. In the second test, the same movement
    is done except the knight captures a piece to test if the hashes are different.
*/
int test_hash() {
    board_t board;
    initialize_testing_board(&board);

    board.to_move = BLACK;
    board.can_castle = BLACK_CASTLE_OO;
    board.pieces[E8] = BLACK_KING;
    board.pieces[H8] = BLACK_ROOK;
    board.pieces[B8] = BLACK_KNIGHT;
    board.pieces[C3] = WHITE_ROOK;
    board.pieces[D1] = WHITE_KING;
    board.piece_bitboard[BLACK_ROOK] = SQUARE_MASK[H8];
    board.piece_bitboard[BLACK_KNIGHT] = SQUARE_MASK[B8];
    board.piece_bitboard[WHITE_ROOK] = SQUARE_MASK[C3];
    board.king_position[WHITE] = D1;
    board.king_position[BLACK] = E8;
    board.color_bitboard[BLACK] = board.piece_bitboard[BLACK_ROOK] | board.piece_bitboard[BLACK_KNIGHT] | SQUARE_MASK[board.king_position[BLACK]];
    board.color_bitboard[WHITE] = board.piece_bitboard[WHITE_ROOK] | SQUARE_MASK[board.king_position[WHITE]];
    board.all_pieces_bitboard = board.color_bitboard[WHITE] | board.color_bitboard[BLACK];

    key_t key = board.hash_key;

    make_move(&board, move_from_text("b8c6", &board)); board.to_move = BLACK;
    make_move(&board, move_from_text("c6e7", &board)); board.to_move = BLACK;
    make_move(&board, move_from_text("e7d5", &board)); board.to_move = BLACK;

    make_move(&board, move_from_text("d5e7", &board)); board.to_move = BLACK;
    make_move(&board, move_from_text("e7c6", &board)); board.to_move = BLACK;
    make_move(&board, move_from_text("c6b8", &board)); board.to_move = BLACK;

    if (key != board.hash_key) {
        fprintf(stderr, "test_hash -- Test 0 failed.");
        return TEST_ERROR;
    }

    make_move(&board, move_from_text("b8c6", &board)); board.to_move = BLACK;
    make_move(&board, move_from_text("c6e7", &board)); board.to_move = BLACK;
    make_move(&board, move_from_text("e7d5", &board)); board.to_move = BLACK;

    make_move(&board, move_from_text("d5c3", &board)); board.to_move = BLACK;
    make_move(&board, move_from_text("c3d5", &board)); board.to_move = BLACK;

    make_move(&board, move_from_text("d5e7", &board)); board.to_move = BLACK;
    make_move(&board, move_from_text("e7c6", &board)); board.to_move = BLACK;
    make_move(&board, move_from_text("c6b8", &board)); board.to_move = BLACK;

    if (key == board.hash_key) {
        fprintf(stderr, "test_hash -- Test 1 failed.");
        return TEST_ERROR;
    }

    return TEST_SUCCESS;
}
