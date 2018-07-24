#include "utilities.h"

/*
    Generate random 64-bit numbers. Make sure rand_init() is called
    before any calls to rand_64(). Based on Bob Jenkins's code.
*/
static struct { unsigned long long int a, b, c, d; } rand_container;

void rand_init(unsigned long long int seed) {
    rand_container.a = 0xF1EA5EED;
    rand_container.b = rand_container.c = rand_container.d = seed;
    for (int i = 0; i < 23; i++) {
        rand_64();
    }
}

unsigned long long int rand_64() {
    #define rotate(x, k) (((x)<<(k))|((x)>>(64-(k))))
    unsigned long long int e = rand_container.a - rotate(rand_container.b, 7);
    rand_container.a = rand_container.b ^ rotate(rand_container.c, 13);
    rand_container.b = rand_container.c + rotate(rand_container.d, 37);
    rand_container.c = rand_container.d + e;
    rand_container.d = e + rand_container.a;
    return rand_container.d;
    #undef rotate
}
