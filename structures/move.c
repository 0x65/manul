#include <stdio.h>

#include "../common.h"

#include "move.h"

void print_move(move_t move) {
    int from_square = MOVE_FROM(move), to_square = MOVE_TO(move);

    if (IS_CASTLE(move)) {
        if (to_square > from_square) {
            printf("O-O");
        } else {
            printf("O-O-O");
        }
    } else {
        printf("%c%d%c%d", FILE_OF(from_square)+'a', RANK_OF(from_square)+1, FILE_OF(to_square)+'a', RANK_OF(to_square)+1);

        if (IS_PROMOTION(move)) {
            switch (PROMOTION_PIECE(move)) {
                case WHITE_QUEEN:
                case BLACK_QUEEN: printf("q"); break;
                case WHITE_KNIGHT:
                case BLACK_KNIGHT: printf("n"); break;
                case WHITE_ROOK:
                case BLACK_ROOK: printf("r"); break;
                case WHITE_BISHOP:
                case BLACK_BISHOP: printf("b"); break;
                default: break;
            }
        }
    }

    printf("\n");
}


unsigned char piece_type_from_char(char piece_char) {
    switch (piece_char) {
        case 'P': return WHITE_PAWN;
        case 'p': return BLACK_PAWN;
        case 'R': return WHITE_ROOK;
        case 'r': return BLACK_ROOK;
        case 'N': return WHITE_KNIGHT;
        case 'n': return BLACK_KNIGHT;
        case 'B': return WHITE_BISHOP;
        case 'b': return BLACK_BISHOP;
        case 'Q': return WHITE_QUEEN;
        case 'q': return BLACK_QUEEN;
        case 'K': return WHITE_KING;
        case 'k': return BLACK_KING;
    }

    return EMPTY;
}
