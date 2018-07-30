#include <time.h>

#include "../utilities.h"

#include "hash.h"

void hash_init() {
    rand_init((uint64_t)time(NULL));

    for (int i = 0; i < 64; i++) {
        hash_keys.en_passant_targets[i] = rand_64();

        for (int j = 0; j < 16; j++) {
            hash_keys.keys[i][j] = rand_64();
        }
    }

    hash_keys.side = rand_64();
    hash_keys.white_king_side_castle = rand_64();
    hash_keys.white_queen_side_castle = rand_64();
    hash_keys.black_king_side_castle = rand_64();
    hash_keys.black_queen_side_castle = rand_64();
}
