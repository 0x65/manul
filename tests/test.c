#include "test.h"

/*
    Run the entire test suite, and return ERROR if any one of the tests fail, SUCCESS otherwise.
*/
int run_all_tests() {
    if (test_make_undo_move() == ERROR) return ERROR;
    if (test_hash() == ERROR) return ERROR;
    if (test_generate_pseudolegal_moves() == ERROR) return ERROR;
    if (test_repetition_count() == ERROR) return ERROR;
    return SUCCESS;
}


/*
    Zeroes a board for testing purposes.
*/
void initialize_testing_board(board_t* board) {
    hash_init();

    for (int i = 0; i < 11; i++) board->piece_bitboard[i] = 0;
    for (int i = A1; i < H8; i++) board->pieces[i] = EMPTY;
    board->to_move = WHITE;
    board->can_castle = board->hash_key = board->passed_square = board->fifty_move_rule_counter = board->state_index = 0;
    board->king_position[WHITE] = board->king_position[BLACK] = A1;
    board->material[WHITE] = board->material[BLACK] = 0;
    board->color_bitboard[WHITE] = board->color_bitboard[BLACK] = board->all_pieces_bitboard = 0;
}
