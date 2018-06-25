#include "evaluate.h"

/*
    The main evaluation function. Returns a score for the board position in centipawns.
*/
int evaluate(const board_t* board) {
    unsigned int total_material = board->material[WHITE] + board->material[BLACK];
    unsigned int gamestage = GAMESTAGE_LOOKUP[total_material];
    int score = (board->material[WHITE]*100) - (board->material[BLACK]*100);

    if (is_theoretical_draw(board)) return DRAW_SCORE;

    // the winning side should prefer exchanges
    if (score > 0) score += (78 - total_material) >> 1;
    else if (score < 0) score -= (78 - total_material) >> 1;

    // get individual piece bonuses
    for (int square = 0; square < 64; square++) {
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
    if (board->piece_bitboard[WHITE_BISHOP] && (board->piece_bitboard[WHITE_BISHOP]&WHITE_SQUARE_MASK) && (board->piece_bitboard[WHITE_BISHOP]&BLACK_SQUARE_MASK))
        score += BISHOP_PAIR_BONUS;
    if (board->piece_bitboard[BLACK_BISHOP] && (board->piece_bitboard[BLACK_BISHOP]&WHITE_SQUARE_MASK) && (board->piece_bitboard[BLACK_BISHOP]&BLACK_SQUARE_MASK))
        score -= BISHOP_PAIR_BONUS;

    if (board->to_move == WHITE) return score;
    else return -score;
}
