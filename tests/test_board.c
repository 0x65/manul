#include "test_board.h"

/*
    Tests `make_move` and `undo_move` by making and undoing every single move for
    a set of fen positions, then making sure the resulting board is the same as the
    original board.
*/
int test_make_undo_move() {
    static const char* fen_positions[] = { "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
        "1B4n1/5k2/5P2/4p2N/P2QR3/p2pb2K/3Pb2R/8 w - - 0 1", "3b4/N2P1p2/1kP4p/p4n1K/2p5/1PPp2P1/1p6/8 w - - 0 1",
        "8/4p3/3R4/2P4R/P3Pp2/6kp/8/K7 w - - 0 1", "8/8/p3K3/k1N2B2/3P4/6Pp/5P1p/4R3 w - - 0 1",
        "K7/8/1P3r2/1P2b1p1/8/5p2/1Q2q2n/7k b - - 0 1", "1q1rr3/P1RN1P1p/Kp3p2/2P2p2/b4pP1/n4kb1/8/3Rn3 b - - 0 1",
        "8/2P5/Bp4Kn/q7/5P2/k7/8/B7 b - - 0 1", "8/8/1B6/8/P1b4K/8/8/1k6 b - - 0 1",
        "8/P2kp3/7B/1R3r2/1bK4p/Q4pp1/P6N/8 b - - 0 1" };

    board_t board[ELEMENTS_IN(fen_positions)], board_original[ELEMENTS_IN(fen_positions)];
    move_t move_list[256];
    int i;

    for (i = 0; i < ELEMENTS_IN(fen_positions); i++) {
        set_board_from_fen_string(&board[i], fen_positions[i]);
        memcpy(&board_original[i], &board[i], sizeof(board_t));
    }

    clock_t start = clock();
    for (i = 0; i < ELEMENTS_IN(fen_positions); i++) {
        move_t* end_of_list = generate_pseudolegal_moves(&board[i], move_list);
        for (int j = 0; move_list[j] != *end_of_list; j++) {
            make_move(&board[i], move_list[j]);
            undo_move(&board[i], move_list[j]);
            if (!compare_boards(&board[i], &board_original[i])) {
                fprintf(stderr, "test_make_undo_move -- Test \"%s\" failed: move %d.\n", fen_positions[i], j);
                return ERROR;
            }
        }
    }

    return SUCCESS;
}


/*
    Returns 1 if the boards are the same, or 0 if the boards are different. Useful for make/undo test.
    Memcmp can't be used in this case since the board state array will differ.
*/
int compare_boards(board_t* board1, board_t* board2) {
    for (int i = 0; i < NUMBER_OF_PIECES-2; i++) if (board1->piece_bitboard[i] != board2->piece_bitboard[i]) return 0;
    if (board1->color_bitboard[0] != board2->color_bitboard[0]) return 0;
    if (board1->color_bitboard[1] != board2->color_bitboard[1]) return 0;
    if (board1->king_position[WHITE] != board2->king_position[WHITE]) return 0;
    if (board1->king_position[BLACK] != board2->king_position[BLACK]) return 0;
    for (int i = A1; i < H8; i++) if (board1->pieces[i] != board2->pieces[i]) return 0;
    if (board1->to_move != board2->to_move) return 0;
    if (board1->can_castle != board2->can_castle) return 0;
    if (board1->passed_square != board2->passed_square) return 0;
    if (board1->fifty_move_rule_counter != board2->fifty_move_rule_counter) return 0;
    if (board1->material[0] != board2->material[0]) return 0;
    if (board1->material[1] != board2->material[1]) return 0;
    if (board1->hash_key != board2->hash_key) return 0;
    if (board1->state_index != board2->state_index) return 0;
    return 1;
}


/*
    Tests `repetition_count` by making repetitive moves and checking the result.
*/
int test_repetition_count() {
    board_t board;
    initialize_testing_board(&board);
    hash_init();

    board.to_move = BLACK;
    board.pieces[H8] = BLACK_ROOK;
    board.pieces[C3] = WHITE_ROOK;
    board.piece_bitboard[BLACK_ROOK] = SQUARE_MASK[H8];
    board.piece_bitboard[WHITE_ROOK] = SQUARE_MASK[C3];
    board.color_bitboard[BLACK] = board.piece_bitboard[BLACK_ROOK];
    board.color_bitboard[WHITE] = board.piece_bitboard[WHITE_ROOK];
    board.all_pieces_bitboard = board.color_bitboard[WHITE] | board.color_bitboard[BLACK];

    // start position: position has occurred once
    make_move(&board, move_from_text("h8h7", &board));
    make_move(&board, move_from_text("c3c5", &board));
    make_move(&board, move_from_text("h7h8", &board));
    make_move(&board, move_from_text("c5c3", &board)); // after this move: position has occurred twice
    make_move(&board, move_from_text("h8h7", &board));
    make_move(&board, move_from_text("c3c5", &board));
    make_move(&board, move_from_text("h7h8", &board));
    make_move(&board, move_from_text("c5c3", &board)); // after this move: position has occured thrice
    if (repetition_count(&board) != 3) {
        fprintf(stderr, "test_repetition_count -- Test 1 failed.\n");
        return ERROR;
    }
    make_move(&board, move_from_text("h8h7", &board));
    make_move(&board, move_from_text("c3c5", &board));
    make_move(&board, move_from_text("h7h8", &board));
    make_move(&board, move_from_text("c5c3", &board));
    if (repetition_count(&board) != 4) {
        fprintf(stderr, "test_repetition_count -- Test 2 failed.\n");
        return ERROR;
    }
    make_move(&board, move_from_text("h8h6", &board));
    if (repetition_count(&board) != 1) {
        fprintf(stderr, "test_repetition_count -- Test 3 failed.\n");
        return ERROR;
    }

    return SUCCESS;
}
