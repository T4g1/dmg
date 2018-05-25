#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>     // DEBUG

#include "cpu.h"

#include "dmg.h"

cpu_callback l_callback[MAX_OPCODES];

void init(struct dmg_state *s)
{
    s->r16[BC] = (uint16_t*) &s->reg[B];
    s->r16[DE] = (uint16_t*) &s->reg[D];
    s->r16[HL] = (uint16_t*) &s->reg[H];

    // CPU opcode assingation
    l_callback[0x00] = &nop;
    l_callback[0x01] = &nop;
    l_callback[0x02] = &nop;
    l_callback[0x03] = &nop;
    l_callback[0x04] = &nop;
    l_callback[0x05] = &nop;
    l_callback[0x06] = &nop;
    l_callback[0x07] = &nop;
    l_callback[0x08] = &nop;
    l_callback[0x09] = &nop;
    l_callback[0x0A] = &nop;
    l_callback[0x0B] = &nop;
    l_callback[0x0C] = &nop;
    l_callback[0x0D] = &nop;
    l_callback[0x0E] = &nop;
    l_callback[0x0F] = &nop;

    l_callback[0x10] = &nop;
    l_callback[0x11] = &nop;
    l_callback[0x12] = &nop;
    l_callback[0x13] = &nop;
    l_callback[0x14] = &nop;
    l_callback[0x15] = &nop;
    l_callback[0x16] = &nop;
    l_callback[0x17] = &nop;
    l_callback[0x18] = &nop;
    l_callback[0x19] = &nop;
    l_callback[0x1A] = &nop;
    l_callback[0x1B] = &nop;
    l_callback[0x1C] = &nop;
    l_callback[0x1D] = &nop;
    l_callback[0x1E] = &nop;
    l_callback[0x1F] = &nop;

    l_callback[0x20] = &nop;
    l_callback[0x21] = &nop;
    l_callback[0x22] = &nop;
    l_callback[0x23] = &nop;
    l_callback[0x24] = &nop;
    l_callback[0x25] = &nop;
    l_callback[0x26] = &nop;
    l_callback[0x27] = &nop;
    l_callback[0x28] = &nop;
    l_callback[0x29] = &nop;
    l_callback[0x2A] = &nop;
    l_callback[0x2B] = &nop;
    l_callback[0x2C] = &nop;
    l_callback[0x2D] = &nop;
    l_callback[0x2E] = &nop;
    l_callback[0x2F] = &nop;

    l_callback[0x30] = &nop;
    l_callback[0x31] = &ld;
    l_callback[0x32] = &nop;
    l_callback[0x33] = &nop;
    l_callback[0x34] = &nop;
    l_callback[0x35] = &nop;
    l_callback[0x36] = &nop;
    l_callback[0x37] = &nop;
    l_callback[0x38] = &nop;
    l_callback[0x39] = &nop;
    l_callback[0x3A] = &nop;
    l_callback[0x3B] = &nop;
    l_callback[0x3C] = &nop;
    l_callback[0x3D] = &nop;
    l_callback[0x3E] = &nop;
    l_callback[0x3F] = &nop;

    l_callback[0x40] = &nop;
    l_callback[0x41] = &nop;
    l_callback[0x42] = &nop;
    l_callback[0x43] = &nop;
    l_callback[0x44] = &nop;
    l_callback[0x45] = &nop;
    l_callback[0x46] = &nop;
    l_callback[0x47] = &nop;
    l_callback[0x48] = &nop;
    l_callback[0x49] = &nop;
    l_callback[0x4A] = &nop;
    l_callback[0x4B] = &nop;
    l_callback[0x4C] = &nop;
    l_callback[0x4D] = &nop;
    l_callback[0x4E] = &nop;
    l_callback[0x4F] = &nop;

    l_callback[0x50] = &nop;
    l_callback[0x51] = &nop;
    l_callback[0x52] = &nop;
    l_callback[0x53] = &nop;
    l_callback[0x54] = &nop;
    l_callback[0x55] = &nop;
    l_callback[0x56] = &nop;
    l_callback[0x57] = &nop;
    l_callback[0x58] = &nop;
    l_callback[0x59] = &nop;
    l_callback[0x5A] = &nop;
    l_callback[0x5B] = &nop;
    l_callback[0x5C] = &nop;
    l_callback[0x5D] = &nop;
    l_callback[0x5E] = &nop;
    l_callback[0x5F] = &nop;

    l_callback[0x60] = &nop;
    l_callback[0x61] = &nop;
    l_callback[0x62] = &nop;
    l_callback[0x63] = &nop;
    l_callback[0x64] = &nop;
    l_callback[0x65] = &nop;
    l_callback[0x66] = &nop;
    l_callback[0x67] = &nop;
    l_callback[0x68] = &nop;
    l_callback[0x69] = &nop;
    l_callback[0x6A] = &nop;
    l_callback[0x6B] = &nop;
    l_callback[0x6C] = &nop;
    l_callback[0x6D] = &nop;
    l_callback[0x6E] = &nop;
    l_callback[0x6F] = &nop;

    l_callback[0x70] = &nop;
    l_callback[0x71] = &nop;
    l_callback[0x72] = &nop;
    l_callback[0x73] = &nop;
    l_callback[0x74] = &nop;
    l_callback[0x75] = &nop;
    l_callback[0x76] = &nop;
    l_callback[0x77] = &nop;
    l_callback[0x78] = &nop;
    l_callback[0x79] = &nop;
    l_callback[0x7A] = &nop;
    l_callback[0x7B] = &nop;
    l_callback[0x7C] = &nop;
    l_callback[0x7D] = &nop;
    l_callback[0x7E] = &nop;
    l_callback[0x7F] = &nop;

    l_callback[0x80] = &nop;
    l_callback[0x81] = &nop;
    l_callback[0x82] = &nop;
    l_callback[0x83] = &nop;
    l_callback[0x84] = &nop;
    l_callback[0x85] = &nop;
    l_callback[0x86] = &nop;
    l_callback[0x87] = &nop;
    l_callback[0x88] = &nop;
    l_callback[0x89] = &nop;
    l_callback[0x8A] = &nop;
    l_callback[0x8B] = &nop;
    l_callback[0x8C] = &nop;
    l_callback[0x8D] = &nop;
    l_callback[0x8E] = &nop;
    l_callback[0x8F] = &nop;

    l_callback[0x90] = &nop;
    l_callback[0x91] = &nop;
    l_callback[0x92] = &nop;
    l_callback[0x93] = &nop;
    l_callback[0x94] = &nop;
    l_callback[0x95] = &nop;
    l_callback[0x96] = &nop;
    l_callback[0x97] = &nop;
    l_callback[0x98] = &nop;
    l_callback[0x99] = &nop;
    l_callback[0x9A] = &nop;
    l_callback[0x9B] = &nop;
    l_callback[0x9C] = &nop;
    l_callback[0x9D] = &nop;
    l_callback[0x9E] = &nop;
    l_callback[0x9F] = &nop;

    l_callback[0xA0] = &nop;
    l_callback[0xA1] = &nop;
    l_callback[0xA2] = &nop;
    l_callback[0xA3] = &nop;
    l_callback[0xA4] = &nop;
    l_callback[0xA5] = &nop;
    l_callback[0xA6] = &nop;
    l_callback[0xA7] = &nop;
    l_callback[0xA8] = &nop;
    l_callback[0xA9] = &nop;
    l_callback[0xAA] = &nop;
    l_callback[0xAB] = &nop;
    l_callback[0xAC] = &nop;
    l_callback[0xAD] = &nop;
    l_callback[0xAE] = &nop;
    l_callback[0xAF] = &nop;

    l_callback[0xB0] = &nop;
    l_callback[0xB1] = &nop;
    l_callback[0xB2] = &nop;
    l_callback[0xB3] = &nop;
    l_callback[0xB4] = &nop;
    l_callback[0xB5] = &nop;
    l_callback[0xB6] = &nop;
    l_callback[0xB7] = &nop;
    l_callback[0xB8] = &nop;
    l_callback[0xB9] = &nop;
    l_callback[0xBA] = &nop;
    l_callback[0xBB] = &nop;
    l_callback[0xBC] = &nop;
    l_callback[0xBD] = &nop;
    l_callback[0xBE] = &nop;
    l_callback[0xBF] = &nop;

    l_callback[0xC0] = &nop;
    l_callback[0xC1] = &nop;
    l_callback[0xC2] = &nop;
    l_callback[0xC3] = &nop;
    l_callback[0xC4] = &nop;
    l_callback[0xC5] = &nop;
    l_callback[0xC6] = &nop;
    l_callback[0xC7] = &nop;
    l_callback[0xC8] = &nop;
    l_callback[0xC9] = &nop;
    l_callback[0xCA] = &nop;
    l_callback[0xCB] = &nop;
    l_callback[0xCC] = &nop;
    l_callback[0xCD] = &nop;
    l_callback[0xCE] = &nop;
    l_callback[0xCF] = &nop;

    l_callback[0xD0] = &nop;
    l_callback[0xD1] = &nop;
    l_callback[0xD2] = &nop;
    l_callback[0xD3] = &nop;
    l_callback[0xD4] = &nop;
    l_callback[0xD5] = &nop;
    l_callback[0xD6] = &nop;
    l_callback[0xD7] = &nop;
    l_callback[0xD8] = &nop;
    l_callback[0xD9] = &nop;
    l_callback[0xDA] = &nop;
    l_callback[0xDB] = &nop;
    l_callback[0xDC] = &nop;
    l_callback[0xDD] = &nop;
    l_callback[0xDE] = &nop;
    l_callback[0xDF] = &nop;

    l_callback[0xE0] = &nop;
    l_callback[0xE1] = &nop;
    l_callback[0xE2] = &nop;
    l_callback[0xE3] = &nop;
    l_callback[0xE4] = &nop;
    l_callback[0xE5] = &nop;
    l_callback[0xE6] = &nop;
    l_callback[0xE7] = &nop;
    l_callback[0xE8] = &nop;
    l_callback[0xE9] = &nop;
    l_callback[0xEA] = &nop;
    l_callback[0xEB] = &nop;
    l_callback[0xEC] = &nop;
    l_callback[0xED] = &nop;
    l_callback[0xEE] = &nop;
    l_callback[0xEF] = &nop;

    l_callback[0xF0] = &nop;
    l_callback[0xF1] = &nop;
    l_callback[0xF2] = &nop;
    l_callback[0xF3] = &nop;
    l_callback[0xF4] = &nop;
    l_callback[0xF5] = &nop;
    l_callback[0xF6] = &nop;
    l_callback[0xF7] = &nop;
    l_callback[0xF8] = &nop;
    l_callback[0xF9] = &nop;
    l_callback[0xFA] = &nop;
    l_callback[0xFB] = &nop;
    l_callback[0xFC] = &nop;
    l_callback[0xFD] = &nop;
    l_callback[0xFE] = &nop;
    l_callback[0xFF] = &nop;
}

void reset(struct dmg_state *s)
{
    s->reg[A] = 0x01;
    s->reg[F] = 0xB0;
    s->reg[B] = 0x00;
    s->reg[C] = 0x13;
    s->reg[D] = 0x00;
    s->reg[E] = 0xD8;
    s->reg[H] = 0x01;
    s->reg[L] = 0x4D;

    s->SP = 0xFFFE;

    s->ram[0xFF05] = 0x00;     //<! TIMA
    s->ram[0xFF06] = 0x00;     //<! TMA
    s->ram[0xFF07] = 0x00;     //<! TAC
    s->ram[0xFF10] = 0x80;     //<! NR10
    s->ram[0xFF11] = 0xBF;     //<! NR11
    s->ram[0xFF12] = 0xF3;     //<! NR12
    s->ram[0xFF14] = 0xBF;     //<! NR14
    s->ram[0xFF16] = 0x3F;     //<! NR21
    s->ram[0xFF17] = 0x00;     //<! NR22
    s->ram[0xFF19] = 0xBF;     //<! NR24
    s->ram[0xFF1A] = 0x7F;     //<! NR30
    s->ram[0xFF1B] = 0xFF;     //<! NR31
    s->ram[0xFF1C] = 0x9F;     //<! NR32
    s->ram[0xFF1E] = 0xBF;     //<! NR33
    s->ram[0xFF20] = 0xFF;     //<! NR41
    s->ram[0xFF21] = 0x00;     //<! NR42
    s->ram[0xFF22] = 0x00;     //<! NR43
    s->ram[0xFF23] = 0xBF;     //<! NR44
    s->ram[0xFF23] = 0x77;     //<! NR50
    s->ram[0xFF23] = 0xF3;     //<! NR51
    s->ram[0xFF23] = 0xF1;     //<! NR52
    s->ram[0xFF23] = 0x91;     //<! LCDC
    s->ram[0xFF23] = 0x00;     //<! SCY
    s->ram[0xFF23] = 0x00;     //<! SCX
    s->ram[0xFF23] = 0x00;     //<! LYC
    s->ram[0xFF23] = 0xFC;     //<! BGP
    s->ram[0xFF23] = 0xFF;     //<! OBP0
    s->ram[0xFF23] = 0xFF;     //<! OBP1
    s->ram[0xFF23] = 0x00;     //<! WY
    s->ram[0xFF23] = 0x00;     //<! WX
    s->ram[0xFF23] = 0x00;     //<! IE
}

bool load(const char *filepath, struct dmg_state *s, uint16_t dst)
{
    FILE *f = fopen(filepath, "rb");
    if (f == NULL) {
        fprintf(stderr, "Unable to read provided boot ROM file\n");
        return false;
    }

    uint8_t byte;
    while (fread(&byte, sizeof(uint8_t), 1, f) == 1) {
        s->ram[dst++] = byte;
    }

    // Failure of reading not EOF related
    if (!feof(f)) {
        fprintf(stderr, "Error while reading provided boot ROM file\n");
        return false;
    }

    return true;
}

int main(int argc, char *argv[])
{
    if (argc != 3) {
        fprintf(stderr, "Usage: dmg <BOOT ROM> <GAME ROM>\n");
        return EXIT_FAILURE;
    }

    char *boot_rom = argv[1];
    //char *game_rom = argv[2];

    struct dmg_state s;
    s.clock = 0;
    s.PC = 0x0000;

    init(&s);
    reset(&s);

    bool running = true;

    fprintf(stdout, "Loading boot ROM from %s\n", boot_rom);
    running &= load(boot_rom, &s, 0x0000);

    //fprintf("Loading game ROM from %s\n", game_rom);

    while (running) {
        fprintf(stdout, "PC: %d\n", s.PC);

        uint8_t opcode = s.ram[s.PC];
        (*l_callback[opcode])(&s, opcode);

        // DEBUG
        sleep(1);
    }

    return EXIT_SUCCESS;
}
