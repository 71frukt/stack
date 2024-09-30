#ifndef STACK_CANARY_H
#define STACK_CANARY_H

typedef long long int canary_t;

const canary_t CANARY_VALUE = 987654321; 

#ifdef DEBUG
#define CANARY_SIZE  sizeof(canary_t)

#else
#define CANARY_SIZE  0

#endif

#endif