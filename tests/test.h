#ifndef _TEST_H
#define _TEST_H

#include "../structures/board.h"

#define TEST_SUCCESS (0)
#define TEST_ERROR (1)

int run_all_tests();
void initialize_testing_board(board_t* board);
#endif
