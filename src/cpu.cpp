#include <stdio.h>
#include <string.h>

#include "utils.h"

#include "cpu.h"

CPU::CPU(MMU *mmu) : mmu(mmu)
{
    PC = 0;
    clock = 0;

    // CPU opcode assingation
    l_callback[0x00] = &nop;
    l_callback[0x01] = &ld;
    l_callback[0x02] = &ld;
    l_callback[0x03] = NULL;
    l_callback[0x04] = NULL;
    l_callback[0x05] = NULL;
    l_callback[0x06] = NULL;
    l_callback[0x07] = NULL;
    l_callback[0x08] = NULL;
    l_callback[0x09] = NULL;
    l_callback[0x0A] = NULL;
    l_callback[0x0B] = NULL;
    l_callback[0x0C] = NULL;
    l_callback[0x0D] = NULL;
    l_callback[0x0E] = NULL;
    l_callback[0x0F] = NULL;

    l_callback[0x10] = NULL;
    l_callback[0x11] = &ld;
    l_callback[0x12] = &ld;
    l_callback[0x13] = NULL;
    l_callback[0x14] = NULL;
    l_callback[0x15] = NULL;
    l_callback[0x16] = NULL;
    l_callback[0x17] = NULL;
    l_callback[0x18] = NULL;
    l_callback[0x19] = NULL;
    l_callback[0x1A] = NULL;
    l_callback[0x1B] = NULL;
    l_callback[0x1C] = NULL;
    l_callback[0x1D] = NULL;
    l_callback[0x1E] = NULL;
    l_callback[0x1F] = NULL;

    l_callback[0x20] = NULL;
    l_callback[0x21] = &ld;
    l_callback[0x22] = &ld;
    l_callback[0x23] = NULL;
    l_callback[0x24] = NULL;
    l_callback[0x25] = NULL;
    l_callback[0x26] = NULL;
    l_callback[0x27] = NULL;
    l_callback[0x28] = NULL;
    l_callback[0x29] = NULL;
    l_callback[0x2A] = NULL;
    l_callback[0x2B] = NULL;
    l_callback[0x2C] = NULL;
    l_callback[0x2D] = NULL;
    l_callback[0x2E] = NULL;
    l_callback[0x2F] = NULL;

    l_callback[0x30] = NULL;
    l_callback[0x31] = &ld;
    l_callback[0x32] = &ld;
    l_callback[0x33] = NULL;
    l_callback[0x34] = NULL;
    l_callback[0x35] = NULL;
    l_callback[0x36] = NULL;
    l_callback[0x37] = NULL;
    l_callback[0x38] = NULL;
    l_callback[0x39] = NULL;
    l_callback[0x3A] = NULL;
    l_callback[0x3B] = NULL;
    l_callback[0x3C] = NULL;
    l_callback[0x3D] = NULL;
    l_callback[0x3E] = NULL;
    l_callback[0x3F] = NULL;

    l_callback[0x40] = NULL;
    l_callback[0x41] = NULL;
    l_callback[0x42] = NULL;
    l_callback[0x43] = NULL;
    l_callback[0x44] = NULL;
    l_callback[0x45] = NULL;
    l_callback[0x46] = NULL;
    l_callback[0x47] = NULL;
    l_callback[0x48] = NULL;
    l_callback[0x49] = NULL;
    l_callback[0x4A] = NULL;
    l_callback[0x4B] = NULL;
    l_callback[0x4C] = NULL;
    l_callback[0x4D] = NULL;
    l_callback[0x4E] = NULL;
    l_callback[0x4F] = NULL;

    l_callback[0x50] = NULL;
    l_callback[0x51] = NULL;
    l_callback[0x52] = NULL;
    l_callback[0x53] = NULL;
    l_callback[0x54] = NULL;
    l_callback[0x55] = NULL;
    l_callback[0x56] = NULL;
    l_callback[0x57] = NULL;
    l_callback[0x58] = NULL;
    l_callback[0x59] = NULL;
    l_callback[0x5A] = NULL;
    l_callback[0x5B] = NULL;
    l_callback[0x5C] = NULL;
    l_callback[0x5D] = NULL;
    l_callback[0x5E] = NULL;
    l_callback[0x5F] = NULL;

    l_callback[0x60] = NULL;
    l_callback[0x61] = NULL;
    l_callback[0x62] = NULL;
    l_callback[0x63] = NULL;
    l_callback[0x64] = NULL;
    l_callback[0x65] = NULL;
    l_callback[0x66] = NULL;
    l_callback[0x67] = NULL;
    l_callback[0x68] = NULL;
    l_callback[0x69] = NULL;
    l_callback[0x6A] = NULL;
    l_callback[0x6B] = NULL;
    l_callback[0x6C] = NULL;
    l_callback[0x6D] = NULL;
    l_callback[0x6E] = NULL;
    l_callback[0x6F] = NULL;

    l_callback[0x70] = NULL;
    l_callback[0x71] = NULL;
    l_callback[0x72] = NULL;
    l_callback[0x73] = NULL;
    l_callback[0x74] = NULL;
    l_callback[0x75] = NULL;
    l_callback[0x76] = NULL;
    l_callback[0x77] = NULL;
    l_callback[0x78] = NULL;
    l_callback[0x79] = NULL;
    l_callback[0x7A] = NULL;
    l_callback[0x7B] = NULL;
    l_callback[0x7C] = NULL;
    l_callback[0x7D] = NULL;
    l_callback[0x7E] = NULL;
    l_callback[0x7F] = NULL;

    l_callback[0x80] = NULL;
    l_callback[0x81] = NULL;
    l_callback[0x82] = NULL;
    l_callback[0x83] = NULL;
    l_callback[0x84] = NULL;
    l_callback[0x85] = NULL;
    l_callback[0x86] = NULL;
    l_callback[0x87] = NULL;
    l_callback[0x88] = NULL;
    l_callback[0x89] = NULL;
    l_callback[0x8A] = NULL;
    l_callback[0x8B] = NULL;
    l_callback[0x8C] = NULL;
    l_callback[0x8D] = NULL;
    l_callback[0x8E] = NULL;
    l_callback[0x8F] = NULL;

    l_callback[0x90] = NULL;
    l_callback[0x91] = NULL;
    l_callback[0x92] = NULL;
    l_callback[0x93] = NULL;
    l_callback[0x94] = NULL;
    l_callback[0x95] = NULL;
    l_callback[0x96] = NULL;
    l_callback[0x97] = NULL;
    l_callback[0x98] = NULL;
    l_callback[0x99] = NULL;
    l_callback[0x9A] = NULL;
    l_callback[0x9B] = NULL;
    l_callback[0x9C] = NULL;
    l_callback[0x9D] = NULL;
    l_callback[0x9E] = NULL;
    l_callback[0x9F] = NULL;

    l_callback[0xA0] = NULL;
    l_callback[0xA1] = NULL;
    l_callback[0xA2] = NULL;
    l_callback[0xA3] = NULL;
    l_callback[0xA4] = NULL;
    l_callback[0xA5] = NULL;
    l_callback[0xA6] = NULL;
    l_callback[0xA7] = NULL;
    l_callback[0xA8] = NULL;
    l_callback[0xA9] = NULL;
    l_callback[0xAA] = NULL;
    l_callback[0xAB] = NULL;
    l_callback[0xAC] = NULL;
    l_callback[0xAD] = NULL;
    l_callback[0xAE] = NULL;
    l_callback[0xAF] = &_xor;

    l_callback[0xB0] = NULL;
    l_callback[0xB1] = NULL;
    l_callback[0xB2] = NULL;
    l_callback[0xB3] = NULL;
    l_callback[0xB4] = NULL;
    l_callback[0xB5] = NULL;
    l_callback[0xB6] = NULL;
    l_callback[0xB7] = NULL;
    l_callback[0xB8] = NULL;
    l_callback[0xB9] = NULL;
    l_callback[0xBA] = NULL;
    l_callback[0xBB] = NULL;
    l_callback[0xBC] = NULL;
    l_callback[0xBD] = NULL;
    l_callback[0xBE] = NULL;
    l_callback[0xBF] = NULL;

    l_callback[0xC0] = NULL;
    l_callback[0xC1] = NULL;
    l_callback[0xC2] = NULL;
    l_callback[0xC3] = NULL;
    l_callback[0xC4] = NULL;
    l_callback[0xC5] = NULL;
    l_callback[0xC6] = NULL;
    l_callback[0xC7] = NULL;
    l_callback[0xC8] = NULL;
    l_callback[0xC9] = NULL;
    l_callback[0xCA] = NULL;
    l_callback[0xCB] = NULL;
    l_callback[0xCC] = NULL;
    l_callback[0xCD] = NULL;
    l_callback[0xCE] = NULL;
    l_callback[0xCF] = NULL;

    l_callback[0xD0] = NULL;
    l_callback[0xD1] = NULL;
    l_callback[0xD2] = NULL;
    l_callback[0xD3] = NULL;
    l_callback[0xD4] = NULL;
    l_callback[0xD5] = NULL;
    l_callback[0xD6] = NULL;
    l_callback[0xD7] = NULL;
    l_callback[0xD8] = NULL;
    l_callback[0xD9] = NULL;
    l_callback[0xDA] = NULL;
    l_callback[0xDB] = NULL;
    l_callback[0xDC] = NULL;
    l_callback[0xDD] = NULL;
    l_callback[0xDE] = NULL;
    l_callback[0xDF] = NULL;

    l_callback[0xE0] = NULL;
    l_callback[0xE1] = NULL;
    l_callback[0xE2] = NULL;
    l_callback[0xE3] = NULL;
    l_callback[0xE4] = NULL;
    l_callback[0xE5] = NULL;
    l_callback[0xE6] = NULL;
    l_callback[0xE7] = NULL;
    l_callback[0xE8] = NULL;
    l_callback[0xE9] = NULL;
    l_callback[0xEA] = NULL;
    l_callback[0xEB] = NULL;
    l_callback[0xEC] = NULL;
    l_callback[0xED] = NULL;
    l_callback[0xEE] = NULL;
    l_callback[0xEF] = NULL;

    l_callback[0xF0] = NULL;
    l_callback[0xF1] = NULL;
    l_callback[0xF2] = NULL;
    l_callback[0xF3] = NULL;
    l_callback[0xF4] = NULL;
    l_callback[0xF5] = NULL;
    l_callback[0xF6] = NULL;
    l_callback[0xF7] = NULL;
    l_callback[0xF8] = NULL;
    l_callback[0xF9] = NULL;
    l_callback[0xFA] = NULL;
    l_callback[0xFB] = NULL;
    l_callback[0xFC] = NULL;
    l_callback[0xFD] = NULL;
    l_callback[0xFE] = NULL;
    l_callback[0xFF] = NULL;
}

void CPU::reset()
{
    reg[A] = 0x01;
    reg[F] = 0xB0;
    reg[B] = 0x00;
    reg[C] = 0x13;
    reg[D] = 0x00;
    reg[E] = 0xD8;
    reg[H] = 0x01;
    reg[L] = 0x4D;

    SP = 0xFFFE;

    mmu->set(0xFF05, 0x00);     //<! TIMA
    mmu->set(0xFF06, 0x00);     //<! TMA
    mmu->set(0xFF07, 0x00);     //<! TAC
    mmu->set(0xFF10, 0x80);     //<! NR10
    mmu->set(0xFF11, 0xBF);     //<! NR11
    mmu->set(0xFF12, 0xF3);     //<! NR12
    mmu->set(0xFF14, 0xBF);     //<! NR14
    mmu->set(0xFF16, 0x3F);     //<! NR21
    mmu->set(0xFF17, 0x00);     //<! NR22
    mmu->set(0xFF19, 0xBF);     //<! NR24
    mmu->set(0xFF1A, 0x7F);     //<! NR30
    mmu->set(0xFF1B, 0xFF);     //<! NR31
    mmu->set(0xFF1C, 0x9F);     //<! NR32
    mmu->set(0xFF1E, 0xBF);     //<! NR33
    mmu->set(0xFF20, 0xFF);     //<! NR41
    mmu->set(0xFF21, 0x00);     //<! NR42
    mmu->set(0xFF22, 0x00);     //<! NR43
    mmu->set(0xFF23, 0xBF);     //<! NR44
    mmu->set(0xFF23, 0x77);     //<! NR50
    mmu->set(0xFF23, 0xF3);     //<! NR51
    mmu->set(0xFF23, 0xF1);     //<! NR52
    mmu->set(0xFF23, 0x91);     //<! LCDC
    mmu->set(0xFF23, 0x00);     //<! SCY
    mmu->set(0xFF23, 0x00);     //<! SCX
    mmu->set(0xFF23, 0x00);     //<! LYC
    mmu->set(0xFF23, 0xFC);     //<! BGP
    mmu->set(0xFF23, 0xFF);     //<! OBP0
    mmu->set(0xFF23, 0xFF);     //<! OBP1
    mmu->set(0xFF23, 0x00);     //<! WY
    mmu->set(0xFF23, 0x00);     //<! WX
    mmu->set(0xFF23, 0x00);     //<! IE
}

void CPU::step()
{
    uint8_t opcode = mmu->get(PC);

    fprintf(stdout, "PC: %08d\tClock: %d\tOpcode: %02X\n", PC, (int)clock, opcode);

    // DEBUG: Should crash if NULL
    if (l_callback[opcode] == NULL) {
        PC += 1; // Just in case
        return;
    }

    (*this.*l_callback[opcode])();
}

void CPU::ld8(void *dst, void* src, size_t size, size_t ticks)
{
    memcpy(dst, src, sizeof(uint8_t));
    PC += size;
    clock += ticks;

    fprintf(stdout, "LD reg\n");
}

void CPU::ld16(void *dst, void* src, size_t size, size_t ticks)
{
    memcpy(dst, src, sizeof(uint16_t));
    PC += size;
    clock += ticks;

    fprintf(stdout, "LD r16\n");
}

void CPU::ld()
{
    uint8_t opcode = mmu->get(PC);
    switch (opcode) {
    case 0x01:    // Loads 16-bit immediate to BC
        ld16(&reg[BC], mmu->at(PC + 1), 3, 12);
        break;

    case 0x11:    // Loads 16-bit immediate to DE
        ld16(&reg[DE], mmu->at(PC + 1), 3, 12);
        break;

    case 0x21:    // Loads 16-bit immediate to HL
        ld16(&reg[HL], mmu->at(PC + 1), 3, 12);
        break;

    case 0x31:    // Loads 16-bit immediate to SP
        ld16(&SP, mmu->at(PC + 1), 3, 12);
        break;

    case 0x02:    // Loads reg A to (BC)
        ld8(mmu->at(reg[BC]), &reg[A], 1, 8);
        break;

    case 0x12:    // Loads reg A to (DE)
        ld8(mmu->at(reg[DE]), &reg[A], 1, 8);
        break;

    case 0x22:    // Loads reg A to (HL), inc HL
        ld8(mmu->at(reg[HL]++), &reg[A], 1, 8);
        break;

    case 0x32:    // Loads reg A to (HL), dec HL
        ld8(mmu->at(reg[HL]--), &reg[A], 1, 8);
        break;
    }
}

void CPU::nop()
{
    PC += 1;
    clock += 4;
}

void CPU::_xor()
{
    uint8_t opcode = mmu->get(PC);
    switch (opcode) {
    case 0xAF:    // XOR A
    default:
        reg[A] ^= reg[A];

        reg[F] = set_bit(reg[F], FZ, reg[A] == 0);
        reg[F] = set_bit(reg[F], FH, 0);
        reg[F] = set_bit(reg[F], FN, 0);
        reg[F] = set_bit(reg[F], FC, 0);

        PC += 1;
        clock += 4;

        fprintf(stdout, "XOR A\n");
    }
}
