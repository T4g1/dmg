#include <stdio.h>

#include "utils.h"


bool get_bit(uint8_t byte, size_t offset)
{
    return  byte & (1 << offset);
}

uint8_t set_bit(uint8_t byte, size_t offset, bool value)
{
    if (value) {
        return byte | (1 << offset);
    } else {
        return byte & ~(1 << offset);
    }
}

bool rotate(uint8_t byte, bool left, bool *carry)
{
    if (left) {
        *carry = get_bit(byte, 7);
        return byte << 1;
    } else {
        *carry = get_bit(byte, 0);
        return byte >> 1;
    }
}

bool shift(uint8_t byte, bool left, bool *carry)
{
    if (left) {
        *carry = get_bit(byte, 7);
        return byte << 1;
    } else {
        *carry = get_bit(byte, 0);
        return byte >> 1;
    }
}

uint8_t swap(uint8_t byte)
{
    uint8_t tmp = byte;

    byte = byte >> 4;
    tmp = tmp << 4;

    return byte | tmp;
}

void dec16(uint8_t *address)
{
    uint8_t *high = address;
    uint8_t *low = address + 1;
    *low -= 1;
    if (*low == 0xFF) {
        *high -= 1;
    }
}

void inc16(uint8_t *address)
{
    uint8_t *high = address;
    uint8_t *low = address + 1;
    *low += 1;
    if (*low == 0x00) {
        *high += 1;
    }
}
