#ifndef UTILS_H
#define UTILS_H

#include <inttypes.h>
#include <stdbool.h>

bool get_bit(uint8_t byte, size_t offset);
uint8_t set_bit(uint8_t byte, size_t offset, bool value);
bool rotate(uint8_t byte, bool left, bool *carry);
bool shift(uint8_t byte, bool left, bool *carry);
uint8_t swap(uint8_t byte);

#endif /* UTILS_H */
