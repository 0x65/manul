#ifndef _TEST_MOVES_H
#define _TEST_MOVES_H

#include <assert.h>
#include <stdio.h>

#include "test.h"
#include "../moves.h"
#include "../structures/board.h"
#include "../structures/move.h"

#define ELEMENTS_IN(array) (sizeof array/sizeof *array)

int test_generate_pseudolegal_moves();
#endif
