#include <stdio.h>
#include <string.h>

#include "cpu.h"

void nop(struct dmg_state *s, uint8_t opcode)
{
    s->PC += 1;
    s->clock += 4;
}

void ld(struct dmg_state *s, uint8_t opcode)
{
    switch (opcode) {
    case 31:    // Loads 16-bit immediate to SP
    default:
        memcpy(&s->SP, s->ram + s->PC + 1, sizeof(uint16_t));
        s->PC += 3;
        s->clock += 12;

        fprintf(stdout, "LD immediate to SP\n");
    }
}
