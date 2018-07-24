#include <locale.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <wchar.h>

#include "evaluation/constants.h"
#include "evaluation/evaluate.h"
#include "structures/bitboard.h"
#include "structures/board.h"
#include "structures/hash.h"
#include "tests/test.h"

#include "common.h"
#include "masks.h"
#include "moves.h"
#include "search.h"
#include "utilities.h"


int main(int argc, char* argv[]) {
    setlocale(LC_ALL, "");
    hash_init();

    // TODO: separate into separate makefile target
    run_all_tests();

    char buffer[64];
    int len;

    move_t move = 0, last_move = 0;
    board_t board;
    RESET_BOARD(board);

    while (1) {
    top:
        print_board(&board);
        move = 0;

        while (!move) {
            printf("enter move> ");
            fgets(buffer, 64, stdin);

            len = strlen(buffer);
            if (buffer[len-1] == '\n') {
                buffer[len-1] = '\0';
            }

            // this is mostly for switching sides right now
            // TODO: implement real undo w/ full move history
            if (!strncmp(buffer, "undo", 4)) {
                if (last_move != 0) {
                    undo_move(&board, last_move);
                    last_move = 0;
                }

                goto top;
            } else if (!strncmp(buffer, "exit", 4)) {
                return 0;
            }

            move = move_from_text(buffer, &board);
        }

        make_move(&board, move);
        print_board(&board);

        printf("Thinking...\n");
        last_move = think(&board);
        make_move(&board, last_move);
        print_move(last_move);
    }

    return 0;
}
