#include "board.h"

#define UPDATE_CASTLE_BITBOARDS(color, rook, to, from) board->piece_bitboard[rook] ^= SQUARE_MASK[to] | SQUARE_MASK[from]; \
        board->color_bitboard[color] ^= SQUARE_MASK[to] | SQUARE_MASK[from]; \
        board->pieces[from] = EMPTY; \
        board->pieces[to] = rook;

/*
    Plays a move on a board. The move is assumed to be legal. 
*/
void make_move(board_t* board, move_t move) {
    #define MAKE_CASTLE(color, removed_castle_bits, king_side_from, king_side_to, queen_side_from, queen_side_to) \
        board->can_castle &= removed_castle_bits; \
        board->king_position[color] = to_square; \
        if (to_square > from_square) { \
            UPDATE_CASTLE_BITBOARDS(color, color##_ROOK, king_side_to, king_side_from); \
            board->hash_key ^= (hash_keys.keys[king_side_to][color##_ROOK] ^ hash_keys.keys[king_side_from][color##_ROOK]); \
        } else { \
            UPDATE_CASTLE_BITBOARDS(color, color##_ROOK, queen_side_to, queen_side_from); \
            board->hash_key ^= (hash_keys.keys[queen_side_to][color##_ROOK] ^ hash_keys.keys[queen_side_from][color##_ROOK]); }

    #define MAKE_CAPTURE(other_color, square_offset) int captured_square = to_square; \
                if (IS_ENPASSANT(move)) { \
                    captured_square += square_offset; \
                    board->pieces[captured_square] = EMPTY; } \
                board->color_bitboard[other_color] ^= SQUARE_MASK[captured_square]; \
                board->piece_bitboard[captured_piece] ^= SQUARE_MASK[captured_square]; \
                board->fifty_move_rule_counter = 0; \
                board->material[other_color] -= MATERIAL_VALUE[captured_piece]; \
                board->hash_key ^= hash_keys.keys[captured_square][captured_piece];

    #define MAKE_PAWN_MOVE(color, square_offset) \
                if ((from_square^to_square) == 16) { \
                    board->passed_square = (to_square - square_offset); \
                    board->hash_key ^= hash_keys.en_passant_targets[board->passed_square]; }\
                if (IS_PROMOTION(move)) { \
                    board->pieces[to_square] = promotion_piece; \
                    board->piece_bitboard[color##_PAWN] ^= SQUARE_MASK[to_square]; \
                    board->piece_bitboard[promotion_piece] |= SQUARE_MASK[to_square]; \
                    board->material[color] += MATERIAL_VALUE[promotion_piece] - 1; \
                    board->hash_key ^= (hash_keys.keys[to_square][color##_PAWN] ^ hash_keys.keys[to_square][promotion_piece]); } \
                board->fifty_move_rule_counter = 0;

    board->state[board->state_index].move = move;
    board->state[board->state_index].can_castle = board->can_castle;
    board->state[board->state_index].passed_square = board->passed_square;
    board->state[board->state_index].fifty_move_rule_counter = board->fifty_move_rule_counter;
    board->state[board->state_index].material[WHITE] = board->material[WHITE];
    board->state[board->state_index].material[BLACK] = board->material[BLACK];
    board->state[board->state_index].hash_key = board->hash_key;

    board->fifty_move_rule_counter++;
    if (board->passed_square) board->hash_key ^= hash_keys.en_passant_targets[board->passed_square];
    board->passed_square = 0;

    int from_square = MOVE_FROM(move), to_square = MOVE_TO(move), moving_piece = MOVING_PIECE(move), 
        captured_piece = CAPTURED_PIECE(move), promotion_piece = PROMOTION_PIECE(move);
    bitboard_t from_to_bitboard = SQUARE_MASK[from_square] | SQUARE_MASK[to_square];

    board->pieces[from_square] = EMPTY;
    board->pieces[to_square] = moving_piece;
    board->hash_key ^= (hash_keys.keys[from_square][moving_piece] ^ hash_keys.keys[to_square][moving_piece]);

    if (board->to_move == WHITE) {
        board->color_bitboard[WHITE] ^= from_to_bitboard;

        if (IS_CASTLE(move)) { MAKE_CASTLE(WHITE, 0xC, H1, F1, A1, D1); }
        if (captured_piece) { MAKE_CAPTURE(BLACK, -8); }
        if (moving_piece == WHITE_PAWN) { MAKE_PAWN_MOVE(WHITE, 8); }

        if (moving_piece == WHITE_KING) board->king_position[WHITE] = to_square;
        else board->piece_bitboard[moving_piece] ^= from_to_bitboard;

        board->to_move = BLACK;
    } else {
        board->color_bitboard[BLACK] ^= from_to_bitboard;

        if (IS_CASTLE(move)) { MAKE_CASTLE(BLACK, 0x3, H8, F8, A8, D8); }
        if (captured_piece) { MAKE_CAPTURE(WHITE, 8); }
        if (moving_piece == BLACK_PAWN) { MAKE_PAWN_MOVE(BLACK, -8); }

        if (moving_piece == BLACK_KING) board->king_position[BLACK] = to_square;
        else board->piece_bitboard[moving_piece] ^= from_to_bitboard;

        board->to_move = WHITE;
    }

    board->hash_key ^= hash_keys.side;
    board->can_castle &= FROM_SQUARE_CASTLING_RIGHTS_MASK[from_square];
    if (board->can_castle != board->state[board->state_index].can_castle) {
        if ((board->can_castle & WHITE_CASTLE_OO) != (board->state[board->state_index].can_castle & WHITE_CASTLE_OO)) board->hash_key ^= hash_keys.white_king_side_castle;
        if ((board->can_castle & WHITE_CASTLE_OOO) != (board->state[board->state_index].can_castle & WHITE_CASTLE_OOO)) board->hash_key ^= hash_keys.white_queen_side_castle;
        if ((board->can_castle & BLACK_CASTLE_OO) != (board->state[board->state_index].can_castle & BLACK_CASTLE_OO)) board->hash_key ^= hash_keys.black_king_side_castle;
        if ((board->can_castle & BLACK_CASTLE_OOO) != (board->state[board->state_index].can_castle & BLACK_CASTLE_OOO)) board->hash_key ^= hash_keys.black_queen_side_castle;
    }
    board->all_pieces_bitboard = board->color_bitboard[WHITE] | board->color_bitboard[BLACK];
    board->state_index++;
}


/*
    Undoes a move on a board.
*/
void undo_move(board_t* board, move_t move) {
    #define UNDO_CASTLE(color, king_side_from, king_side_to, queen_side_from, queen_side_to) \
        board->king_position[color] = from_square; \
        if (to_square > from_square) { UPDATE_CASTLE_BITBOARDS(color, color##_ROOK, king_side_from, king_side_to); } \
        else { UPDATE_CASTLE_BITBOARDS(color, color##_ROOK, queen_side_from, queen_side_to); }

    #define UNDO_CAPTURE(other_color, square_offset) int captured_square = to_square; \
                if (IS_ENPASSANT(move)) { \
                    captured_square += square_offset; \
                } \
                board->color_bitboard[other_color] |= SQUARE_MASK[captured_square]; \
                board->piece_bitboard[captured_piece] |= SQUARE_MASK[captured_square]; \
                board->pieces[captured_square] = captured_piece;

    #define UNDO_PROMOTION(pawn) board->piece_bitboard[promotion_piece] ^= SQUARE_MASK[to_square]; \
                board->piece_bitboard[pawn] |= SQUARE_MASK[to_square];

    board->state_index--;
    board->can_castle = board->state[board->state_index].can_castle;
    board->passed_square = board->state[board->state_index].passed_square;
    board->fifty_move_rule_counter = board->state[board->state_index].fifty_move_rule_counter;
    board->material[WHITE] = board->state[board->state_index].material[WHITE];
    board->material[BLACK] = board->state[board->state_index].material[BLACK];
    board->hash_key = board->state[board->state_index].hash_key;

    int from_square = MOVE_FROM(move), to_square = MOVE_TO(move), moving_piece = MOVING_PIECE(move), 
        captured_piece = CAPTURED_PIECE(move), promotion_piece = PROMOTION_PIECE(move);
    bitboard_t from_to_bitboard = SQUARE_MASK[from_square] | SQUARE_MASK[to_square];

    board->pieces[from_square] = moving_piece;
    board->pieces[to_square] = EMPTY;

    if (board->to_move == BLACK) {
        board->color_bitboard[WHITE] ^= from_to_bitboard;

        if (IS_CASTLE(move)) { UNDO_CASTLE(WHITE, H1, F1, A1, D1); }
        if (IS_PROMOTION(move)) { UNDO_PROMOTION(WHITE_PAWN); }
        if (captured_piece) {  UNDO_CAPTURE(BLACK, -8); }

        if (moving_piece == WHITE_KING) board->king_position[WHITE] = from_square;
        else board->piece_bitboard[moving_piece] ^= from_to_bitboard;

        board->to_move = WHITE;
    } else {
        board->color_bitboard[BLACK] ^= from_to_bitboard;

        if (IS_CASTLE(move)) { UNDO_CASTLE(BLACK, H8, F8, A8, D8); }
        if (IS_PROMOTION(move)) { UNDO_PROMOTION(BLACK_PAWN); }
        if (captured_piece) {  UNDO_CAPTURE(WHITE, 8); }

        if (moving_piece == BLACK_KING) board->king_position[BLACK] = from_square;
        else board->piece_bitboard[moving_piece] ^= from_to_bitboard;

        board->to_move = BLACK;
    }

    board->all_pieces_bitboard = board->color_bitboard[WHITE] | board->color_bitboard[BLACK];
}


/*
    Sets up a board to the position indicated by the FEN string.
*/
void set_board_from_fen_string(board_t* board, const char* fen_string) {
    int fen_index = 0, fen_length = strlen(fen_string), square;
    char current_char;

    for (square=A1; square <= H8; square++) board->pieces[square] = EMPTY;
    for (int i = 0; i < NUMBER_OF_PIECES-2; i++) board->piece_bitboard[i] = 0;
    board->color_bitboard[WHITE] = board->color_bitboard[BLACK] = 0;
    board->can_castle = 0;
    board->passed_square = 0;
    board->fifty_move_rule_counter = 0;
    board->material[WHITE] = board->material[BLACK] = 0;
    board->hash_key = 0;
    board->state_index = 0;

    // set pieces
    for (int rank = 7; rank >= 0; rank--) {
        int file = 0;
        do {
            if (file > 7) { fprintf(stderr, "FEN read error: too many files\n"); return; }
            current_char = fen_string[fen_index++];
            if (current_char >= '1' && current_char <= '8') file += (current_char - '0');
            else {
                board->pieces[rank*8+file] = piece_type_from_char(current_char);
                file++;
            }
        } while (fen_string[fen_index] != '/' && fen_string[fen_index] != ' ');
        fen_index++;
    }
    if (fen_string[fen_index-1] != ' ') { fprintf(stderr, "FEN read error: too many ranks\n"); return; }

    // set side to move
    if (fen_string[fen_index] == 'W' || fen_string[fen_index] == 'w') board->to_move = WHITE;
    else if (fen_string[fen_index] == 'B' || fen_string[fen_index] == 'b') {
        board->to_move = BLACK;
        board->hash_key ^= hash_keys.side;
    } else { fprintf(stderr, "FEN read error: invalid side to move\n"); return; }
    fen_index += 2;

    // set castling permissions
    if (fen_index < fen_length) {
        do {
            switch (fen_string[fen_index]) {
                case 'K':
                    board->can_castle |= WHITE_CASTLE_OO;
                    board->hash_key ^= hash_keys.white_king_side_castle;
                    break;
                case 'Q':
                    board->can_castle |= WHITE_CASTLE_OOO;
                    board->hash_key ^= hash_keys.white_queen_side_castle;
                    break;
                case 'k':
                    board->can_castle |= BLACK_CASTLE_OO;
                    board->hash_key ^= hash_keys.black_king_side_castle;
                    break;
                case 'q':
                    board->can_castle |= BLACK_CASTLE_OOO;
                    board->hash_key ^= hash_keys.black_queen_side_castle;
                    break;
                case '-': break;
                default: fprintf(stderr, "FEN read error: invalid castle permission\n"); return;
            }
        } while (fen_string[++fen_index] != ' ' && fen_index < fen_length);
    }

    // set en-passant square
    fen_index++;
    if (fen_index+1 < fen_length && fen_string[fen_index] != '-') {
        board->passed_square = (fen_string[fen_index]-'a'+1) + (fen_string[fen_index+1]-'0')*8 - 9;
        if (board->passed_square < A1 || board->passed_square > H8) { fprintf(stderr, "FEN read error: invalid en-passant square\n"); return; }
        board->hash_key ^= hash_keys.en_passant_targets[board->passed_square];
    }

    // set up bitboards & material values
    for (square=A1; square <= H8; square++) {
        switch(board->pieces[square]) {
            case WHITE_KING:
                board->king_position[WHITE] = square;
                board->hash_key ^= hash_keys.keys[square][WHITE_KING];
                break;
            case BLACK_KING:
                board->king_position[BLACK] = square;
                board->hash_key ^= hash_keys.keys[square][BLACK_KING];
                break;
            default:
                board->piece_bitboard[board->pieces[square]] |= SQUARE_MASK[square];
                board->material[COLOR_OF_PIECE(board->pieces[square])] += MATERIAL_VALUE[board->pieces[square]];
                board->hash_key ^= hash_keys.keys[square][board->pieces[square]];
                break;
        }
    }
    board->color_bitboard[WHITE] = board->piece_bitboard[WHITE_PAWN] | board->piece_bitboard[WHITE_ROOK] | board->piece_bitboard[WHITE_BISHOP] |
        board->piece_bitboard[WHITE_KNIGHT] | board->piece_bitboard[WHITE_QUEEN] | SQUARE_MASK[board->king_position[WHITE]];
    board->color_bitboard[BLACK] = board->piece_bitboard[BLACK_PAWN] | board->piece_bitboard[BLACK_ROOK] | board->piece_bitboard[BLACK_BISHOP] |
        board->piece_bitboard[BLACK_KNIGHT] | board->piece_bitboard[BLACK_QUEEN] | SQUARE_MASK[board->king_position[BLACK]];
    board->all_pieces_bitboard = board->color_bitboard[WHITE] | board->color_bitboard[BLACK];
}


/*
    Prints a human-readable form of a chessboard. White pieces are represented by capital letters,
    black ones by lowercase.
*/
void print_board(const board_t* board) {
    for (int file = 0; file < 8; file++) {
        printf("%d |", 8-file);
        for (int rank = 0; rank < 8; rank++) {
            switch (board->pieces[(7-file)*8+rank]) {
                case EMPTY: wprintf(L" ."); break;
                case WHITE_PAWN: wprintf(L" ♙"); break;
                case BLACK_PAWN: wprintf(L" ♟"); break;
                case WHITE_ROOK: wprintf(L" ♖"); break;
                case BLACK_ROOK: wprintf(L" ♜"); break;
                case WHITE_KNIGHT: wprintf(L" ♘"); break;
                case BLACK_KNIGHT: wprintf(L" ♞"); break;
                case WHITE_BISHOP: wprintf(L" ♗"); break;
                case BLACK_BISHOP: wprintf(L" ♝"); break;
                case WHITE_QUEEN: wprintf(L" ♕"); break;
                case BLACK_QUEEN: wprintf(L" ♛"); break;
                case WHITE_KING: wprintf(L" ♔"); break;
                case BLACK_KING: wprintf(L" ♚"); break;
            }
        }
        printf("\n");
    }
    printf("   ----------------\n    a b c d e f g h\n");
}


/*
    Makes a move_t from a supplied string (in long algebraic notation), or
    returns 0 if it's an invalid move.
*/
move_t move_from_text(const char* text, board_t* board) {
    int text_length = strlen(text);
    if (text_length != 4 && text_length != 5) return 0;

    move_t move = (((text[1]-'1')*8)+(tolower(text[0])-'a')) + ((((text[3]-'1')*8)+(tolower(text[2])-'a'))<<6);
    if (text_length == 5)
        if (board->to_move == WHITE)
            move |= piece_type_from_char(toupper(text[4]))<<23;
        else
            move |= piece_type_from_char(tolower(text[4]))<<23;

    move_t move_list[MAX_MOVES];
    move_t* end_of_move_list = generate_pseudolegal_moves(board, move_list);
    for (int i = 0; &move_list[i] != end_of_move_list; i++) {
        if ((move_list[i]&0x7800FFF) == move) {
            if (IS_CASTLE(move_list[i]) && !is_legal_castle(board, move_list[i])) return 0;
            make_move(board, move_list[i]);
            if (!is_king_in_check(board, !board->to_move)) {
                undo_move(board, move_list[i]);
                return move_list[i];
            }
       }
    }

    return 0;
}


/*
    Returns the number of repetitions on a board by comparing every other hash key.
*/
unsigned int repetition_count(const board_t* board) {
    unsigned int repetitions = 1;
    for (int i = board->state_index - 2, last_i = board->state_index - board->fifty_move_rule_counter; i >= last_i; i -= 2)
        if (board->state[i].hash_key == board->hash_key) repetitions++;
    return repetitions;
}
