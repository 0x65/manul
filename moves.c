#include <ctype.h>
#include <string.h>
#include <wchar.h>

#include "structures/bitboard.h"

#include "common.h"
#include "masks.h"
#include "moves.h"

#define RANK_MOVES(x) (RANK_ATTACK_MASK[x][(board->all_pieces_bitboard&PARTIAL_RANK_BY_SQUARE_MASK[x])>>SHIFT_RANK_TO_INDEX[x]])
#define FILE_MOVES(x) (FILE_ATTACK_MASK[x][((board->all_pieces_bitboard&PARTIAL_FILE_BY_SQUARE_MASK[x])*MULTIPLY_FILE_TO_INDEX[x])>>0x39])
#define A8H1_DIAG_MOVES(x) (A8H1_DIAG_ATTACK_MASK[x][((board->all_pieces_bitboard&PARTIAL_A8H1_DIAG_BY_SQUARE_MASK[x])*MULTIPLY_A8H1_DIAG_TO_INDEX[x])>>0x39])
#define A1H8_DIAG_MOVES(x) (A1H8_DIAG_ATTACK_MASK[x][((board->all_pieces_bitboard&PARTIAL_A1H8_DIAG_BY_SQUARE_MASK[x])*MULTIPLY_A1H8_DIAG_TO_INDEX[x])>>0x39])

#define KNIGHT_MOVES(x) (KNIGHT_MOVE_MASK[x])
#define ROOK_MOVES(x) (RANK_MOVES(x) | FILE_MOVES(x))
#define BISHOP_MOVES(x) (A8H1_DIAG_MOVES(x) | A1H8_DIAG_MOVES(x))
#define QUEEN_MOVES(x) (ROOK_MOVES(x) | BISHOP_MOVES(x))

/*
    Generates all pseudolegal moves for a position described by `board`, and stores the resulting moves
    in `move_list`. Assumes `move_list` has enough allocated memory to store all generated moves. Returns
    a pointer to the end of `move_list` after all moves have been generated.
*/
move_t* generate_pseudolegal_moves(const board_t* board, move_t* move_list) {
    unsigned int from_square = 0, to_square = 0;
    bitboard_t existing_pieces, targets, possible_moves;

    #define GENERATE_MOVES(from_square, moves, piece_name) \
        possible_moves = targets & moves; \
        while (possible_moves) { \
            to_square = FIRST_BIT_SET(possible_moves); \
            *move_list++ = (from_square) + (to_square << 6) + (piece_name << 12) + (board->pieces[to_square] << 16); \
            REMOVE_FIRST_BIT_SET(possible_moves); \
        }

    #define GENERATE_MOVES_FOR_ALL(move_macro, piece_name) \
        existing_pieces = board->piece_bitboard[piece_name]; \
        while (existing_pieces) { \
            from_square = FIRST_BIT_SET(existing_pieces); \
            GENERATE_MOVES(from_square, move_macro(from_square), piece_name); \
            REMOVE_FIRST_BIT_SET(existing_pieces); \
        }

    #define GENERATE_PAWN_MOVES(color, other_color, double_rank, promotion_rank) \
            existing_pieces = board->piece_bitboard[color##_PAWN]; \
            while (existing_pieces) { \
                from_square = FIRST_BIT_SET(existing_pieces); \
                possible_moves = color##_PAWN_MOVE_MASK[from_square] & (~board->all_pieces_bitboard); \
                if (RANK_OF(from_square) == double_rank && possible_moves) { \
                    possible_moves |= color##_PAWN_DOUBLE_MOVE_MASK[from_square] & (~board->all_pieces_bitboard); \
                } \
                possible_moves |= color##_PAWN_ATTACK_MASK[from_square] & board->color_bitboard[other_color]; \
                while (possible_moves) { \
                    to_square = FIRST_BIT_SET(possible_moves); \
                    if (RANK_OF(to_square) == promotion_rank) { \
                        move_t promotion = (from_square) + (to_square << 6) + (color##_PAWN << 12) + (board->pieces[to_square] << 16) + 0x400000; \
                        *move_list++ = promotion + (color##_KNIGHT << 23); \
                        *move_list++ = promotion + (color##_ROOK << 23); \
                        *move_list++ = promotion + (color##_BISHOP << 23); \
                        *move_list++ = promotion + (color##_QUEEN << 23); \
                    } else { \
                        *move_list++ = (from_square) + (to_square << 6) + (color##_PAWN << 12) + (board->pieces[to_square] << 16); \
                    } \
                    REMOVE_FIRST_BIT_SET(possible_moves); \
                } \
                if (board->passed_square && (color##_PAWN_ATTACK_MASK[from_square] & SQUARE_MASK[board->passed_square])) { \
                    *move_list++ = (from_square) + (board->passed_square << 6) + (color##_PAWN << 12) + (other_color##_PAWN << 16) + 0x100000; \
                } \
                REMOVE_FIRST_BIT_SET(existing_pieces); \
            }

    if (board->to_move == WHITE) {
        targets = ~board->color_bitboard[WHITE];

        if ((board->can_castle & WHITE_CASTLE_OO) && !(board->all_pieces_bitboard & 0x60)) {
            *move_list++ = 0x20B184;
        }

        if ((board->can_castle & WHITE_CASTLE_OOO) && !(board->all_pieces_bitboard & 0xE)) {
            *move_list++ = 0x20B084;
        }

        GENERATE_MOVES(board->king_position[WHITE], KING_MOVE_MASK[board->king_position[WHITE]], WHITE_KING);
        GENERATE_MOVES_FOR_ALL(KNIGHT_MOVES, WHITE_KNIGHT);
        GENERATE_MOVES_FOR_ALL(ROOK_MOVES, WHITE_ROOK);
        GENERATE_MOVES_FOR_ALL(BISHOP_MOVES, WHITE_BISHOP);
        GENERATE_MOVES_FOR_ALL(QUEEN_MOVES, WHITE_QUEEN);
        GENERATE_PAWN_MOVES(WHITE, BLACK, RANK_2, RANK_8);
    } else {
        targets = ~board->color_bitboard[BLACK];

        if ((board->can_castle & BLACK_CASTLE_OO) && !(board->all_pieces_bitboard & 0x6000000000000000)) {
            *move_list++ = 0x20CFBC;
        }

        if ((board->can_castle & BLACK_CASTLE_OOO) && !(board->all_pieces_bitboard & 0xE00000000000000)) {
            *move_list++ = 0x20CEBC;
        }

        GENERATE_MOVES(board->king_position[BLACK], KING_MOVE_MASK[board->king_position[BLACK]], BLACK_KING);
        GENERATE_MOVES_FOR_ALL(KNIGHT_MOVES, BLACK_KNIGHT);
        GENERATE_MOVES_FOR_ALL(ROOK_MOVES, BLACK_ROOK);
        GENERATE_MOVES_FOR_ALL(BISHOP_MOVES, BLACK_BISHOP);
        GENERATE_MOVES_FOR_ALL(QUEEN_MOVES, BLACK_QUEEN);
        GENERATE_PAWN_MOVES(BLACK, WHITE, RANK_7, RANK_1);
    }

    #undef GENERATE_MOVES
    #undef GENERATE_MOVES_FOR_ALL
    #undef GENERATE_PAWN_MOVES

    return move_list;
}


/*
    Tests if the `side` king is in check, and returns either the piece type if it is, or 0.
*/
char is_king_in_check(const board_t* board, unsigned int side) {
    #define CHECK(color, other_color) \
        int king_position = board->king_position[color]; \
        bitboard_t diagonal_attacks = BISHOP_MOVES(king_position); \
        bitboard_t sliding_attacks = ROOK_MOVES(king_position); \
        if (KING_MOVE_MASK[king_position] & SQUARE_MASK[board->king_position[other_color]]) return other_color##_KING; \
        if (color##_PAWN_ATTACK_MASK[king_position]&board->piece_bitboard[other_color##_PAWN]) return other_color##_PAWN; \
        if (KNIGHT_MOVES(king_position)&board->piece_bitboard[other_color##_KNIGHT]) return other_color##_KNIGHT; \
        if (diagonal_attacks&board->piece_bitboard[other_color##_QUEEN]) return other_color##_QUEEN; \
        if (diagonal_attacks&board->piece_bitboard[other_color##_BISHOP]) return other_color##_BISHOP; \
        if (sliding_attacks&board->piece_bitboard[other_color##_QUEEN]) return other_color##_QUEEN; \
        if (sliding_attacks&board->piece_bitboard[other_color##_ROOK]) return other_color##_ROOK;

    if (side == WHITE) {
        CHECK(WHITE, BLACK);
    } else {
        CHECK(BLACK, WHITE);
    }

    #undef CHECK

    return 0;
}


/*
    Tests to see if any pieces attack the squares on a castle path. Note that this DOES NOT
    check if the path itself is clear.
*/
bool is_legal_castle(board_t* board, move_t castle_move) {
    unsigned int to_square = MOVE_TO(castle_move), from_square = MOVE_FROM(castle_move);
    move_t test_move;
    char in_check;

    #define CASTLE_CHECK(color) \
        if (is_king_in_check(board, color)) { \
            return false; \
        } \
        if (to_square < from_square) { \
            for (int i = from_square-1; i >= to_square; i--) { \
                test_move = from_square + (i<<6) + (color##_KING<<12); \
                make_move(board, test_move); \
                in_check = is_king_in_check(board, color); \
                undo_move(board, test_move); \
                if (in_check) { \
                    return false; \
                } \
            } \
        } else { \
            for (int i = from_square+1; i <= to_square; i++) { \
                test_move = from_square + (i<<6) + (color##_KING<<12); \
                make_move(board, test_move); \
                in_check = is_king_in_check(board, color); \
                undo_move(board, test_move); \
                if (in_check) { \
                    return false; \
                } \
            } \
        }

    if (board->to_move == WHITE) {
        CASTLE_CHECK(WHITE);
    } else {
        CASTLE_CHECK(BLACK);
    }

    #undef CASTLE_CHECK

    return true;
}


/*
    Returns the number of legal moves for a position.
*/
unsigned int count_legal_moves(board_t* board) {
    move_t move_list[MAX_MOVES];
    unsigned int moves_found = 0;
    move_t* end_of_move_list = generate_pseudolegal_moves(board, move_list);

    for (int i = 0; &move_list[i] != end_of_move_list; i++) {
        if (IS_CASTLE(move_list[i]) && !is_legal_castle(board, move_list[i])) {
            continue;
        }

        make_move(board, move_list[i]);

        if (!is_king_in_check(board, !board->to_move)) {
            moves_found++;
        }

        undo_move(board, move_list[i]);
    }
    return moves_found;
}


/*
    Makes a move_t from a supplied string (in long algebraic notation), or
    returns 0 if it's an invalid move.
*/
move_t move_from_text(const char* text, board_t* board) {
    int text_length = strlen(text);

    if (text_length != 4 && text_length != 5) {
        return 0;
    }

    move_t move = (((text[1]-'1')*8)+(tolower(text[0])-'a')) + ((((text[3]-'1')*8)+(tolower(text[2])-'a'))<<6);

    if (text_length == 5) {
        if (board->to_move == WHITE) {
            move |= piece_type_from_char(toupper(text[4]))<<23;
        } else {
            move |= piece_type_from_char(tolower(text[4]))<<23;
        }
    }

    // check legality
    move_t move_list[MAX_MOVES];
    move_t* end_of_move_list = generate_pseudolegal_moves(board, move_list);

    for (int i = 0; &move_list[i] != end_of_move_list; i++) {
        if ((move_list[i]&0x7800FFF) == move) {
            if (IS_CASTLE(move_list[i]) && !is_legal_castle(board, move_list[i])) {
                return 0;
            }

            make_move(board, move_list[i]);

            if (!is_king_in_check(board, !board->to_move)) {
                undo_move(board, move_list[i]);
                return move_list[i];
            }
       }
    }

    return 0;
}
