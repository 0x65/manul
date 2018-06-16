#ifndef _TEST_H
#define _TEST_H

#define SUCCESS (0)
#define ERROR (1)

#include "test_board.h"
#include "test_hash.h"
#include "test_moves.h"

int run_all_tests();
void initialize_testing_board(board_t* board);
#endif
