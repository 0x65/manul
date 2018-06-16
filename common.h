#ifndef _COMMON_H
#define _COMMON_H

#define NAME "MANUL"
#define VERSION "0.1"

#define WHITE (0)
#define BLACK (1)
#define OPPOSITE_WHITE (BLACK)
#define OPPOSITE_BLACK (WHITE)

#define EMPTY (0)
#define WHITE_PAWN (1)
#define WHITE_ROOK (2)
#define WHITE_KNIGHT (3)
#define WHITE_BISHOP (4)
#define WHITE_QUEEN (5)
#define BLACK_PAWN (6)
#define BLACK_ROOK (7)
#define BLACK_KNIGHT (8)
#define BLACK_BISHOP (9)
#define BLACK_QUEEN (10)
#define WHITE_KING (11)
#define BLACK_KING (12)
#define NUMBER_OF_PIECES (13)

#define COLOR_OF_PIECE(x) ((((x)>5)&&((x)<11))||((x)==12))

#define PAWN_MATERIAL_VALUE (1)
#define KNIGHT_MATERIAL_VALUE (3)
#define ROOK_MATERIAL_VALUE (5)
#define BISHOP_MATERIAL_VALUE (3)
#define QUEEN_MATERIAL_VALUE (9)

static const unsigned int MATERIAL_VALUE[13] = {0, PAWN_MATERIAL_VALUE, ROOK_MATERIAL_VALUE, KNIGHT_MATERIAL_VALUE, BISHOP_MATERIAL_VALUE, QUEEN_MATERIAL_VALUE, PAWN_MATERIAL_VALUE, ROOK_MATERIAL_VALUE, KNIGHT_MATERIAL_VALUE, BISHOP_MATERIAL_VALUE, QUEEN_MATERIAL_VALUE, 0, 0};

#define A1 (0)
#define B1 (1)
#define C1 (2)
#define D1 (3)
#define E1 (4)
#define F1 (5)
#define G1 (6)
#define H1 (7)
#define A2 (8)
#define B2 (9)
#define C2 (10)
#define D2 (11)
#define E2 (12)
#define F2 (13)
#define G2 (14)
#define H2 (15)
#define A3 (16)
#define B3 (17)
#define C3 (18)
#define D3 (19)
#define E3 (20)
#define F3 (21)
#define G3 (22)
#define H3 (23)
#define A4 (24)
#define B4 (25)
#define C4 (26)
#define D4 (27)
#define E4 (28)
#define F4 (29)
#define G4 (30)
#define H4 (31)
#define A5 (32)
#define B5 (33)
#define C5 (34)
#define D5 (35)
#define E5 (36)
#define F5 (37)
#define G5 (38)
#define H5 (39)
#define A6 (40)
#define B6 (41)
#define C6 (42)
#define D6 (43)
#define E6 (44)
#define F6 (45)
#define G6 (46)
#define H6 (47)
#define A7 (48)
#define B7 (49)
#define C7 (50)
#define D7 (51)
#define E7 (52)
#define F7 (53)
#define G7 (54)
#define H7 (55)
#define A8 (56)
#define B8 (57)
#define C8 (58)
#define D8 (59)
#define E8 (60)
#define F8 (61)
#define G8 (62)
#define H8 (63)

#define FILE_A 0
#define FILE_B 1
#define FILE_C 2
#define FILE_D 3
#define FILE_E 4
#define FILE_F 5
#define FILE_G 6
#define FILE_H 7
#define FILE_OF(x) (x&7)

#define RANK_1 0
#define RANK_2 1
#define RANK_3 2
#define RANK_4 3
#define RANK_5 4
#define RANK_6 5
#define RANK_7 6
#define RANK_8 7
#define RANK_OF(x) (x>>3)

#define DRAW_SCORE (0)
#define CHECKMATE_SCORE (100000)

#define OPENING (1)
#define MIDDLE (2)
#define ENDGAME (3)

static const int GAMESTAGE_LOOKUP[128] = { ENDGAME, ENDGAME, ENDGAME, ENDGAME, ENDGAME, ENDGAME, ENDGAME, ENDGAME, ENDGAME, ENDGAME, ENDGAME, ENDGAME, ENDGAME, ENDGAME, ENDGAME, ENDGAME, ENDGAME, ENDGAME, MIDDLE, MIDDLE, MIDDLE, MIDDLE, MIDDLE, MIDDLE, MIDDLE, MIDDLE, MIDDLE, MIDDLE, MIDDLE, MIDDLE, MIDDLE, MIDDLE, MIDDLE, MIDDLE, MIDDLE, MIDDLE, MIDDLE, MIDDLE, MIDDLE, MIDDLE, MIDDLE, MIDDLE, MIDDLE, MIDDLE, MIDDLE, MIDDLE, MIDDLE, MIDDLE, MIDDLE, MIDDLE, MIDDLE, MIDDLE, MIDDLE, MIDDLE, MIDDLE, MIDDLE, MIDDLE, MIDDLE, MIDDLE, MIDDLE, MIDDLE, MIDDLE, MIDDLE, MIDDLE, MIDDLE, MIDDLE, MIDDLE, MIDDLE, MIDDLE, MIDDLE, MIDDLE, OPENING, OPENING, OPENING, OPENING, OPENING, OPENING, OPENING, OPENING, OPENING, OPENING, OPENING, OPENING, OPENING, OPENING, OPENING, OPENING, OPENING, OPENING, OPENING, OPENING, OPENING, OPENING, OPENING, OPENING, OPENING, OPENING, OPENING, OPENING, OPENING, OPENING, OPENING, OPENING, OPENING, OPENING, OPENING, OPENING, OPENING, OPENING, OPENING, OPENING, OPENING, OPENING, OPENING, OPENING, OPENING, OPENING, OPENING, OPENING, OPENING, OPENING, OPENING, OPENING, OPENING, OPENING, OPENING, OPENING, OPENING };
#endif
