#include "../masks.h"

#include "strategy.h"

#define POSITION_AND_KING_DISTANCE_BONUS(color, piece) score += color##_##piece##_POSITION_BONUS[square]; \
    score += piece##_DISTANCE_TO_KING_BONUS[DISTANCE[square][board->king_position[OPPOSITE_##color]]];

/*
    Evaluates the score of a pawn in position `square`.
*/
int evaluate_pawn(unsigned int square, const board_t* board, unsigned int side, int gamestage) {
    int score = 0;

    #define IS_PAWN_CLOSE_TO_KING_IN_ENDGAME(color) if (gamestage >= ENDGAME) \
        score += PAWN_DISTANCE_TO_OWN_KING_ENDGAME_BONUS[DISTANCE[square][board->king_position[color]]];

    #define IS_PASSED_PAWN(color, other_color) \
        if (!(color##_PASSED_PAWN_OBSTACLE_MASK[square] & board->piece_bitboard[other_color##_PAWN])) \
            score += PASSED_PAWN_BONUS;

    #define IS_DOUBLED_PAWN(color) if ((board->piece_bitboard[color##_PAWN] ^ SQUARE_MASK[square])&FILE_BY_SQUARE_MASK[square]) \
        score += DOUBLED_PAWN_PENALTY;

    #define IS_ISOLATED_OR_BACKWARD_PAWN(color, other_color) if (!(ADJACENT_PAWN_MASK[square]&board->piece_bitboard[color##_PAWN])) \
            score += ISOLATED_PAWN_PENALTY; \
        else \
            /* next square is controlled by an enemy pawn */ \
            if ((WHITE_PAWN_ATTACK_MASK[square+8]&board->piece_bitboard[other_color##_PAWN])) \
                /* there are no pawns to defend this one behind it */ \
                if (!(ADJACENT_POSTERIOR_##color##_PAWN_MASK[square]&board->piece_bitboard[color##_PAWN])) \
                    score += BACKWARD_PAWN_PENALTY;

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

    return score;
}


/*
    Evaluates the score of a knight in position `square`.
*/
int evaluate_knight(unsigned int square, const board_t* board, unsigned int side, int gamestage) {
    int score = 0;
    if (side == WHITE) { POSITION_AND_KING_DISTANCE_BONUS(WHITE, KNIGHT); }
    else { POSITION_AND_KING_DISTANCE_BONUS(BLACK, KNIGHT); }
    return score;
}


/*
    Evaluates the score of a bishop in position `square`.
*/
int evaluate_bishop(unsigned int square, const board_t* board, unsigned int side, int gamestage) {
    int score = 0;
    if (side == WHITE) { POSITION_AND_KING_DISTANCE_BONUS(WHITE, BISHOP); }
    else { POSITION_AND_KING_DISTANCE_BONUS(BLACK, BISHOP); }
    return score;
}


/*
    Evaluates the score of a queen in position `square`.
*/
int evaluate_queen(unsigned int square, const board_t* board, unsigned int side, int gamestage) {
    int score = 0;
    if (side == WHITE) { POSITION_AND_KING_DISTANCE_BONUS(WHITE, QUEEN); }
    else { POSITION_AND_KING_DISTANCE_BONUS(BLACK, QUEEN); }
    return score;
}


/*
    Evaluates the score of a rook in position `square`.
*/
int evaluate_rook(unsigned int square, const board_t* board, unsigned int side, int gamestage) {
    int score = 0;
    unsigned int file_square;

    #define IS_ROOK_BEHIND_PASSED_PAWN(color, other_color, comparator) \
        bitboard_t passed_pawn_candidates = PARTIAL_FILE_BY_SQUARE_MASK[square] & board->piece_bitboard[color##_PAWN]; \
        while (passed_pawn_candidates) { \
            file_square = FIRST_BIT_SET(passed_pawn_candidates); \
            if (!(color##_PASSED_PAWN_OBSTACLE_MASK[file_square] & board->piece_bitboard[other_color##_PAWN])) \
                if (square comparator file_square) \
                    score += ROOK_BEHIND_PASSED_PAWN_BONUS; \
            REMOVE_FIRST_BIT_SET(passed_pawn_candidates); }

    if (side == WHITE) {
        POSITION_AND_KING_DISTANCE_BONUS(WHITE, ROOK);
        IS_ROOK_BEHIND_PASSED_PAWN(WHITE, BLACK, <);
    } else {
        POSITION_AND_KING_DISTANCE_BONUS(BLACK, ROOK);
        IS_ROOK_BEHIND_PASSED_PAWN(BLACK, WHITE, >);
    }

    return score;
}


/*
    Evaluates the score of a king in position `square`.
*/
int evaluate_king(unsigned int square, const board_t* board, unsigned int side, int gamestage) {
    int score = 0;

    #define IS_KING_GUARDED(color, piece, protection_mask, weak_protection_mask) \
        score += KING_PAWN_PROTECTED_BONUS * sparse_bit_count(protection_mask[board->king_position[color]]&board->piece_bitboard[piece]); \
        score += KING_WEAKLY_PAWN_PROTECTED_BONUS * sparse_bit_count(weak_protection_mask[board->king_position[color]]&board->piece_bitboard[piece]);

    #define KING_POSITION(color) if (gamestage >= ENDGAME) { \
            score += KING_ENDGAME_POSITION_BONUS[square]; \
        } else { \
            score += color##_KING_POSITION_BONUS[square]; \
            IS_KING_GUARDED(color, color##_PAWN, color##_KING_PROTECTION_PAWN_MASK, color##_KING_WEAK_PROTECTION_PAWN_MASK); \
        }

    if (side == WHITE) { KING_POSITION(WHITE); }
    else { KING_POSITION(BLACK); }
    return score;
}
