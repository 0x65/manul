#ifndef _HASH_H
#define _HASH_H

#include <time.h>

#include "../utilities.h"

typedef unsigned long long int key_t;

typedef struct _hash_keys {
    key_t keys[64][16]; // [position][piece] 
    key_t en_passant_targets[64];
    key_t side; // if set on a key, represents black's turn to move
    key_t white_king_side_castle, white_queen_side_castle, black_king_side_castle, black_queen_side_castle;
} hash_keys_t;

hash_keys_t hash_keys;

void hash_init();
#endif
