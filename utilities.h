#ifndef _UTILITIES_H
#define _UTILITIES_H

typedef struct _rand_container { unsigned long long int a, b, c, d; } rand_container_t;
static rand_container_t rand_container;

void rand_init(unsigned long long int seed);
unsigned long long int rand_64();
#endif
