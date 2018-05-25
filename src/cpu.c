#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "cpu.h"

uint8_t set_bit(uint8_t byte, size_t offset, bool value)
{
    if (value) {
        return byte | (1 << offset);
    } else {
        return byte & ~(1 << offset);
    }
}

uint16_t get_imm16(struct dmg_state *s, size_t offset)
{
    return s->ram[offset];
}

void nop(struct dmg_state *s, uint8_t opcode)
{
    s->PC += 1;
    s->clock += 4;
}

void ld8(struct dmg_state *s, uint8_t *dst, void* src, size_t size, size_t clock)
{
    memcpy(dst, src, sizeof(uint8_t));
    s->PC += size;
    s->clock += clock;

    fprintf(stdout, "LD reg\n");
}

void ld16(struct dmg_state *s, uint16_t *dst, void* src, size_t size, size_t clock)
{
    memcpy(dst, src, sizeof(uint16_t));
    s->PC += size;
    s->clock += clock;

    fprintf(stdout, "LD r16\n");
}

void ld(struct dmg_state *s, uint8_t opcode)
{
    switch (opcode) {
    case 0x01:    // Loads 16-bit immediate to BC
        ld16(s, s->r16[BC], s->ram + s->PC + 1, 3, 12);
        break;

    case 0x11:    // Loads 16-bit immediate to DE
        ld16(s, s->r16[DE], s->ram + s->PC + 1, 3, 12);
        break;

    case 0x21:    // Loads 16-bit immediate to HL
        ld16(s, s->r16[HL], s->ram + s->PC + 1, 3, 12);
        break;

    case 0x31:    // Loads 16-bit immediate to SP
        ld16(s, &s->SP, s->ram + s->PC + 1, 3, 12);
        break;

    case 0x02:    // Loads reg A to (BC)
        ld8(s, s->ram + *s->r16[BC], &s->reg[A], 1, 8);
        break;

    case 0x12:    // Loads reg A to (DE)
        ld8(s, s->ram + *s->r16[DE], &s->reg[A], 1, 8);
        break;

    case 0x22:    // Loads reg A to (HL), inc HL
        ld8(s, s->ram + *s->r16[HL], &s->reg[A], 1, 8);
        *s->r16[HL] += 1;
        break;

    case 0x32:    // Loads reg A to (HL), dec HL
        ld8(s, s->ram + *s->r16[HL], &s->reg[A], 1, 8);
        *s->r16[HL] -= 1;
        break;
    }
}

void xor(struct dmg_state *s, uint8_t opcode)
{
    switch (opcode) {
    case 0xAF:    // XOR A
    default:
        s->reg[A] ^= s->reg[A];

        s->reg[F] = set_bit(s->reg[F], FZ, s->reg[A] == 0);
        s->reg[F] = set_bit(s->reg[F], FH, 0);
        s->reg[F] = set_bit(s->reg[F], FN, 0);
        s->reg[F] = set_bit(s->reg[F], FC, 0);

        s->PC += 1;
        s->clock += 4;

        fprintf(stdout, "XOR A\n");
    }
}
