#ifndef UTILS_H
#define UTILS_H

#include <inttypes.h>
#include <stdbool.h>

bool get_bit(uint8_t byte, size_t offset);
uint8_t set_bit(uint8_t byte, size_t offset, bool value);
uint8_t rotate(uint8_t byte, bool left, bool *carry);
uint8_t shift(uint8_t byte, bool left, bool *carry);
uint8_t swap(uint8_t byte);
void dec16(uint8_t *address);
void inc16(uint8_t *address);

#endif /* UTILS_H */
