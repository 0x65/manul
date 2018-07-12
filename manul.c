#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "common.h"
#include "masks.h"
#include "moves.h"
#include "structures/bitboard.h"
#include "structures/board.h"
#include "evaluation/evaluate.h"
#include "evaluation/constants.h"
#include "search.h"
#include "utilities.h"
#include "tests/test.h"

#include <locale.h>
#include <wchar.h>

#include "structures/hash.h"
#include <time.h>

int main(int argc, char* argv[]) {
    setlocale(LC_ALL, "");

    board_t board;
    RESET_BOARD(board);
    char buffer[256];
    int len;

    hash_init();
    run_all_tests();
    print_board(&board);

    move_t m;
    while (1) {
    top:
        print_board(&board);
        move_t move = 0;
        while (move == 0)
        { 
            printf("enter move> ");
            fgets(buffer, 255, stdin);
            if (!strcmp(buffer, "undo\n")) {
                undo_move(&board, m);            
                goto top;
            } else if (!strcmp(buffer, "exit\n")) {
                return 0;
            }
            len = strlen(buffer);
            if (buffer[len-1] == '\n') buffer[len-1] = '\0';
            move = move_from_text(buffer, &board);
        }
        make_move(&board, move);
        print_board(&board);

        printf("Calculating...\n");
        m = think(&board);
        make_move(&board, m);
        printf("Done calculating...\n");
        print_move(m);
    }
    return 0;
}
