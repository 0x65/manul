#ifndef _TEST_BOARD_H
#define _TEST_BOARD_H

#include <assert.h>
#include <stdio.h>

#include "test.h"
#include "../moves.h"
#include "../structures/board.h"
#include "../structures/move.h"

#define ELEMENTS_IN(array) (sizeof array/sizeof *array)

int test_make_undo_move();
int test_repetition_count();
int compare_boards(board_t* board1, board_t* board2);
#endif
