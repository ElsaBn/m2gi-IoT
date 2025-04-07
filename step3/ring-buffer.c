#include "ring-buffer.h"

volatile uint32_t tail = 0;
volatile uint8_t buffer[MAX_CHARS];
volatile uint32_t head = 0;


bool_t ring_empty() {
    return (head==tail);
}

bool_t ring_full() {
    int next = (head + 1) % MAX_CHARS;
    return (next==tail);
}

void ring_put(uint8_t bits) {
    uint32_t next = (head + 1) % MAX_CHARS;
    buffer[head] = code;
    head = next;
}

uint8_t ring_get() {
    uint8_t bits;
    uint32_t next = (tail + 1) % MAX_CHARS;
    bits = buffer[tail];
    tail = next;
    return bits;
}
