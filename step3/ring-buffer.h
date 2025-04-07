#ifndef RING_BUFFER_H_
#define RING_BUFFER_H_

#define MAX_CHARS 512

typedef uint8_t bool_t;

bool_t ring_empty();
bool_t ring_full();
void ring_put(uint8_t bits);
uint8_t ring_get();

#endif