#include <stdio.h>

#include "../structures/board.h"
#include "../structures/move.h"
#include "../moves.h"

#include "test.h"
#include "test_moves.h"

#define ELEMENTS_IN(array) (sizeof array/sizeof *array)

/*
    Tests generate_pseudolegal_moves(const board_t*, move_t*) to ensure it finds the correct
    amount of moves for a set of given positions.
*/
int test_generate_pseudolegal_moves() {
    static const char* fen_positions[] = { "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
        "1B4n1/5k2/5P2/4p2N/P2QR3/p2pb2K/3Pb2R/8 w - - 0 1", "3b4/N2P1p2/1kP4p/p4n1K/2p5/1PPp2P1/1p6/8 w - - 0 1",
        "8/4p3/3R4/2P4R/P3Pp2/6kp/8/K7 w - - 0 1", "8/8/p3K3/k1N2B2/3P4/6Pp/5P1p/4R3 w - - 0 1",
        "K7/8/1P3r2/1P2b1p1/8/5p2/1Q2q2n/7k b - - 0 1", "1q1rr3/P1RN1P1p/Kp3p2/2P2p2/b4pP1/n4kb1/8/3Rn3 b - - 0 1",
        "8/2P5/Bp4Kn/q7/5P2/k7/8/B7 b - - 0 1", "8/8/1B6/8/P1b4K/8/8/1k6 b - - 0 1",
        "8/P2kp3/7B/1R3r2/1bK4p/Q4pp1/P6N/8 b - - 0 1" };

    static const unsigned int number_of_correct_moves[] = { 20, 37, 11, 29, 37, 37, 43, 23, 16, 30 };

    board_t generated_positions[ELEMENTS_IN(fen_positions)];
    move_t move_list[256];

    for (int i = 0; i < ELEMENTS_IN(fen_positions); i++) {
        set_board_from_fen_string(&generated_positions[i], fen_positions[i]);
    }

    for (int i = 0; i < ELEMENTS_IN(fen_positions); i++) {
        move_t *end_of_list = generate_pseudolegal_moves(&generated_positions[i], move_list);
        unsigned long number_of_generated_moves = end_of_list - move_list;

        if (number_of_generated_moves != number_of_correct_moves[i]) {
            fprintf(stderr, "test_generate_pseudolegal_moves -- Test \"%s\" failed: got %ld moves, expected %d.\n",
                fen_positions[i], number_of_generated_moves, number_of_correct_moves[i]);

            return TEST_ERROR;
        }
    }

    return TEST_SUCCESS;
}
