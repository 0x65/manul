#include "../common.h"
#include "../masks.h"
#include "../moves.h"
#include "../structures/bitboard.h"

#include "constants.h"
#include "evaluate.h"

/*
    The main evaluation function. Computes a score for a board state in centipawns (positive to
    indicate white advantage, negative to indicate black advantage), then returns its absolute
    value based on which player's turn it is.
*/
int evaluate(const board_t* board) {
    if (is_theoretical_draw(board)) {
        return DRAW_SCORE;
    }

    int score = (board->material[WHITE]*100) - (board->material[BLACK]*100);
    unsigned int total_material = board->material[WHITE] + board->material[BLACK];

    // the side with more material should prefer exchanges
    if (score > 0) {
        score += (78 - total_material) >> 1;
    } else if (score < 0) {
        score -= (78 - total_material) >> 1;
    }

    // get individual piece bonuses
    unsigned int gamestage = GAMESTAGE_LOOKUP[total_material];

    for (int square = A1; square <= H8; square++) {
        switch (board->pieces[square]) {
            case WHITE_PAWN: score += evaluate_pawn(square, board, WHITE, gamestage); break;
            case WHITE_KNIGHT: score += evaluate_knight(square, board, WHITE, gamestage); break;
            case WHITE_BISHOP: score += evaluate_bishop(square, board, WHITE, gamestage); break;
            case WHITE_ROOK: score += evaluate_rook(square, board, WHITE, gamestage); break;
            case WHITE_QUEEN: score += evaluate_queen(square, board, WHITE, gamestage); break;
            case WHITE_KING: score += evaluate_king(square, board, WHITE, gamestage); break;
            case BLACK_PAWN: score -= evaluate_pawn(square, board, BLACK, gamestage); break;
            case BLACK_KNIGHT: score -= evaluate_knight(square, board, BLACK, gamestage); break;
            case BLACK_BISHOP: score -= evaluate_bishop(square, board, BLACK, gamestage); break;
            case BLACK_ROOK: score -= evaluate_rook(square, board, BLACK, gamestage); break;
            case BLACK_QUEEN: score -= evaluate_queen(square, board, BLACK, gamestage); break;
            case BLACK_KING: score -= evaluate_king(square, board, BLACK, gamestage); break;
        }
    }

    // bishop pair bonuses
    if ((board->piece_bitboard[WHITE_BISHOP] & WHITE_SQUARE_MASK) && (board->piece_bitboard[WHITE_BISHOP] & BLACK_SQUARE_MASK)) {
        score += BISHOP_PAIR_BONUS;
    }

    if ((board->piece_bitboard[BLACK_BISHOP] & WHITE_SQUARE_MASK) && (board->piece_bitboard[BLACK_BISHOP] & BLACK_SQUARE_MASK)) {
        score -= BISHOP_PAIR_BONUS;
    }

    if (board->to_move == WHITE) {
        return score;
    } else {
        return -score;
    }
}


#define POSITION_AND_KING_DISTANCE_BONUS(color, piece) \
    score += color##_##piece##_POSITION_BONUS[square]; \
    score += piece##_DISTANCE_TO_KING_BONUS[DISTANCE[square][board->king_position[OPPOSITE_##color]]];


int evaluate_pawn(unsigned int square, const board_t* board, unsigned int side, int gamestage) {
    int score = 0;

    #define IS_PAWN_CLOSE_TO_KING_IN_ENDGAME(color) \
        if (gamestage >= ENDGAME) { \
            score += PAWN_DISTANCE_TO_OWN_KING_ENDGAME_BONUS[DISTANCE[square][board->king_position[color]]]; \
        }

    #define IS_PASSED_PAWN(color, other_color) \
        if (!(color##_PASSED_PAWN_OBSTACLE_MASK[square] & board->piece_bitboard[other_color##_PAWN])) { \
            score += PASSED_PAWN_BONUS; \
        }

    #define IS_DOUBLED_PAWN(color) \
        if ((board->piece_bitboard[color##_PAWN] ^ SQUARE_MASK[square]) & FILE_BY_SQUARE_MASK[square]) { \
            score += DOUBLED_PAWN_PENALTY; \
        }

    #define IS_ISOLATED_OR_BACKWARD_PAWN(color, other_color) \
        if (!(ADJACENT_PAWN_MASK[square]&board->piece_bitboard[color##_PAWN])) { \
            score += ISOLATED_PAWN_PENALTY; \
        } else { \
            /* next square is controlled by an enemy pawn */ \
            if ((WHITE_PAWN_ATTACK_MASK[square+8] & board->piece_bitboard[other_color##_PAWN])) { \
                /* there are no pawns to defend this one behind it */ \
                if (!(ADJACENT_POSTERIOR_##color##_PAWN_MASK[square]&board->piece_bitboard[color##_PAWN])) { \
                    score += BACKWARD_PAWN_PENALTY; \
                } \
            } \
        }

    if (side == WHITE) {
        POSITION_AND_KING_DISTANCE_BONUS(WHITE, PAWN);
        IS_PAWN_CLOSE_TO_KING_IN_ENDGAME(WHITE);
        IS_PASSED_PAWN(WHITE, BLACK);
        IS_DOUBLED_PAWN(WHITE);
        IS_ISOLATED_OR_BACKWARD_PAWN(WHITE, BLACK);
    } else {
        POSITION_AND_KING_DISTANCE_BONUS(BLACK, PAWN);
        IS_PAWN_CLOSE_TO_KING_IN_ENDGAME(BLACK);
        IS_PASSED_PAWN(BLACK, WHITE);
        IS_DOUBLED_PAWN(BLACK);
        IS_ISOLATED_OR_BACKWARD_PAWN(BLACK, WHITE);
    }

    #undef IS_PAWN_CLOSE_TO_KING_IN_ENDGAME
    #undef IS_PASSED_PAWN
    #undef IS_DOUBLED_PAWN
    #undef IS_ISOLATED_OR_BACKWARD_PAWN

    return score;
}


int evaluate_knight(unsigned int square, const board_t* board, unsigned int side, int gamestage) {
    int score = 0;

    if (side == WHITE) {
        POSITION_AND_KING_DISTANCE_BONUS(WHITE, KNIGHT);
    } else {
        POSITION_AND_KING_DISTANCE_BONUS(BLACK, KNIGHT);
    }

    return score;
}


int evaluate_bishop(unsigned int square, const board_t* board, unsigned int side, int gamestage) {
    int score = 0;

    if (side == WHITE) {
        POSITION_AND_KING_DISTANCE_BONUS(WHITE, BISHOP);
    } else {
        POSITION_AND_KING_DISTANCE_BONUS(BLACK, BISHOP);
    }

    return score;
}


int evaluate_queen(unsigned int square, const board_t* board, unsigned int side, int gamestage) {
    int score = 0;

    if (side == WHITE) {
        POSITION_AND_KING_DISTANCE_BONUS(WHITE, QUEEN);
    } else {
        POSITION_AND_KING_DISTANCE_BONUS(BLACK, QUEEN);
    }

    return score;
}


int evaluate_rook(unsigned int square, const board_t* board, unsigned int side, int gamestage) {
    int score = 0;
    unsigned int file_square;

    #define IS_ROOK_BEHIND_PASSED_PAWN(color, other_color, comparator) \
        bitboard_t passed_pawn_candidates = PARTIAL_FILE_BY_SQUARE_MASK[square] & board->piece_bitboard[color##_PAWN]; \
        while (passed_pawn_candidates) { \
            file_square = FIRST_BIT_SET(passed_pawn_candidates); \
            if (!(color##_PASSED_PAWN_OBSTACLE_MASK[file_square] & board->piece_bitboard[other_color##_PAWN])) { \
                if (square comparator file_square) { \
                    score += ROOK_BEHIND_PASSED_PAWN_BONUS; \
                } \
            } \
            REMOVE_FIRST_BIT_SET(passed_pawn_candidates); \
        }

    if (side == WHITE) {
        POSITION_AND_KING_DISTANCE_BONUS(WHITE, ROOK);
        IS_ROOK_BEHIND_PASSED_PAWN(WHITE, BLACK, <);
    } else {
        POSITION_AND_KING_DISTANCE_BONUS(BLACK, ROOK);
        IS_ROOK_BEHIND_PASSED_PAWN(BLACK, WHITE, >);
    }

    #undef IS_ROOK_BEHIND_PASSED_PAWN

    return score;
}


int evaluate_king(unsigned int square, const board_t* board, unsigned int side, int gamestage) {
    int score = 0;

    #define KING_POSITION(color) \
        if (gamestage >= ENDGAME) { \
            score += KING_ENDGAME_POSITION_BONUS[square]; \
        } else { \
            score += color##_KING_POSITION_BONUS[square]; \
            score += KING_PAWN_PROTECTED_BONUS * sparse_bit_count(color##_KING_PROTECTION_PAWN_MASK[board->king_position[color]] & board->piece_bitboard[color##_PAWN]); \
            score += KING_WEAKLY_PAWN_PROTECTED_BONUS * sparse_bit_count(color##_KING_WEAK_PROTECTION_PAWN_MASK[board->king_position[color]] & board->piece_bitboard[color##_PAWN]); \
        }

    if (side == WHITE) {
        KING_POSITION(WHITE);
    } else {
        KING_POSITION(BLACK);
    }

    #undef KING_POSITION

    return score;
}

#undef POSITION_AND_KING_DISTANCE_BONUS
