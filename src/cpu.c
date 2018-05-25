#include "cpu.h"

void nop(uint16_t *PC, int *clock, uint8_t *reg, uint16_t **r16, uint8_t *ram)
{
    *PC += 1;
    *clock += 4;
}
