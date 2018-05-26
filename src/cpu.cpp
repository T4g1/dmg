#include <stdio.h>
#include <string.h>
#include <unistd.h>     // DEBUG

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
    l_callback[0x03] = &inc;
    l_callback[0x04] = &inc;
    l_callback[0x05] = &dec;
    l_callback[0x06] = &ld;
    l_callback[0x07] = &rxa;
    l_callback[0x08] = &ld;
    l_callback[0x09] = &add;
    l_callback[0x0A] = &ld;
    l_callback[0x0B] = &dec;
    l_callback[0x0C] = &inc;
    l_callback[0x0D] = &dec;
    l_callback[0x0E] = &ld;
    l_callback[0x0F] = &rxa;

    l_callback[0x10] = &stop;
    l_callback[0x11] = &ld;
    l_callback[0x12] = &ld;
    l_callback[0x13] = &inc;
    l_callback[0x14] = &inc;
    l_callback[0x15] = &dec;
    l_callback[0x16] = &ld;
    l_callback[0x17] = &rxa;
    l_callback[0x18] = &jr;
    l_callback[0x19] = &add;
    l_callback[0x1A] = &ld;
    l_callback[0x1B] = &dec;
    l_callback[0x1C] = &inc;
    l_callback[0x1D] = &dec;
    l_callback[0x1E] = &ld;
    l_callback[0x1F] = &rxa;

    l_callback[0x20] = &jr;
    l_callback[0x21] = &ld;
    l_callback[0x22] = &ld;
    l_callback[0x23] = &inc;
    l_callback[0x24] = &inc;
    l_callback[0x25] = &dec;
    l_callback[0x26] = &ld;
    l_callback[0x27] = &daa;
    l_callback[0x28] = &jr;
    l_callback[0x29] = &add;
    l_callback[0x2A] = &ld;
    l_callback[0x2B] = &dec;
    l_callback[0x2C] = &inc;
    l_callback[0x2D] = &dec;
    l_callback[0x2E] = &ld;
    l_callback[0x2F] = &cpl;

    l_callback[0x30] = &jr;
    l_callback[0x31] = &ld;
    l_callback[0x32] = &ld;
    l_callback[0x33] = &inc;
    l_callback[0x34] = &inc;
    l_callback[0x35] = &dec;
    l_callback[0x36] = &ld;
    l_callback[0x37] = &scf;
    l_callback[0x38] = &jr;
    l_callback[0x39] = &add;
    l_callback[0x3A] = &ld;
    l_callback[0x3B] = &dec;
    l_callback[0x3C] = &inc;
    l_callback[0x3D] = &dec;
    l_callback[0x3E] = &ld;
    l_callback[0x3F] = &ccf;

    l_callback[0x40] = &ld;
    l_callback[0x41] = &ld;
    l_callback[0x42] = &ld;
    l_callback[0x43] = &ld;
    l_callback[0x44] = &ld;
    l_callback[0x45] = &ld;
    l_callback[0x46] = &ld;
    l_callback[0x47] = &ld;
    l_callback[0x48] = &ld;
    l_callback[0x49] = &ld;
    l_callback[0x4A] = &ld;
    l_callback[0x4B] = &ld;
    l_callback[0x4C] = &ld;
    l_callback[0x4D] = &ld;
    l_callback[0x4E] = &ld;
    l_callback[0x4F] = &ld;

    l_callback[0x50] = &ld;
    l_callback[0x51] = &ld;
    l_callback[0x52] = &ld;
    l_callback[0x53] = &ld;
    l_callback[0x54] = &ld;
    l_callback[0x55] = &ld;
    l_callback[0x56] = &ld;
    l_callback[0x57] = &ld;
    l_callback[0x58] = &ld;
    l_callback[0x59] = &ld;
    l_callback[0x5A] = &ld;
    l_callback[0x5B] = &ld;
    l_callback[0x5C] = &ld;
    l_callback[0x5D] = &ld;
    l_callback[0x5E] = &ld;
    l_callback[0x5F] = &ld;

    l_callback[0x60] = &ld;
    l_callback[0x61] = &ld;
    l_callback[0x62] = &ld;
    l_callback[0x63] = &ld;
    l_callback[0x64] = &ld;
    l_callback[0x65] = &ld;
    l_callback[0x66] = &ld;
    l_callback[0x67] = &ld;
    l_callback[0x68] = &ld;
    l_callback[0x69] = &ld;
    l_callback[0x6A] = &ld;
    l_callback[0x6B] = &ld;
    l_callback[0x6C] = &ld;
    l_callback[0x6D] = &ld;
    l_callback[0x6E] = &ld;
    l_callback[0x6F] = &ld;

    l_callback[0x70] = &ld;
    l_callback[0x71] = &ld;
    l_callback[0x72] = &ld;
    l_callback[0x73] = &ld;
    l_callback[0x74] = &ld;
    l_callback[0x75] = &ld;
    l_callback[0x76] = &halt;
    l_callback[0x77] = &ld;
    l_callback[0x78] = &ld;
    l_callback[0x79] = &ld;
    l_callback[0x7A] = &ld;
    l_callback[0x7B] = &ld;
    l_callback[0x7C] = &ld;
    l_callback[0x7D] = &ld;
    l_callback[0x7E] = &ld;
    l_callback[0x7F] = &ld;

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
    l_callback[0xCB] = &prefix_CB;
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

    fprintf(stdout, "PC: %08X\tClock: %d\tOpcode: %02X\n", PC, (int)clock, opcode);

    // DEBUG: Should crash if NULL
    if (l_callback[opcode] == NULL) {
        PC += 1; // Just in case
        return;
    }

    (*this.*l_callback[opcode])();

    // DEBUG
    if (PC >= 0x000C) {
        sleep(1);
    }
}

void CPU::ld8(void *dst, void* src, size_t size, size_t ticks)
{
    memcpy(dst, src, sizeof(uint8_t));
    PC += size;
    clock += ticks;

    fprintf(stdout, "LD reg\n");
}

void CPU::ld16(uint8_t *dst, uint8_t* src, size_t size, size_t ticks)
{
    // Invert LOW and HIGH bytes
    memcpy(dst + 1, src, sizeof(uint8_t));
    memcpy(dst, src + 1, sizeof(uint8_t));
    PC += size;
    clock += ticks;

    fprintf(stdout, "LD r16\n");
}

void CPU::inc8(uint8_t *address)
{
    *address = *address + 1;

    reg[F] = set_bit(reg[F], FZ, *address == 0);
    reg[F] = set_bit(reg[F], FN, 0);
    reg[F] = set_bit(reg[F], FH, *address & 0x0F == 0x00); // Went from 0xFF to 0x00
}

void CPU::dec8(uint8_t *address)
{
    *address = *address - 1;

    reg[F] = set_bit(reg[F], FZ, *address == 0);
    reg[F] = set_bit(reg[F], FN, 1);
    reg[F] = set_bit(reg[F], FH, *address & 0x0F == 0x0F); // Went from 0x00 to 0xFF
}

void CPU::add16(uint8_t *dst, uint8_t *src)
{
    bool half_carry = false, carry = false;

    uint8_t *dh = dst;
    uint8_t *sh = src;
    uint8_t *dl = dst + 1;
    uint8_t *sl = src + 1;

    int idl = (int)*dl;
    int isl = (int)*sl;
    int tmp = idl + isl;
    if (tmp > 255) {
        half_carry = true;
        *dl = (uint8_t)(tmp & 0xFF);
    }

    int idh = (int)*dh;
    int ish = (int)*sh;
    tmp = idh + ish + half_carry;
    if (tmp > 255) {
        carry = true;
        *dh = (uint8_t)(tmp & 0xFF);
    }

    reg[F] = set_bit(reg[F], FN, 0);
    reg[F] = set_bit(reg[F], FH, half_carry == 1);
    reg[F] = set_bit(reg[F], FC, carry = 1);

    fprintf(stdout, "ADD r16\n");
}

// Invert reg A
void CPU::cpl()
{
    reg[A] = ~reg[A];

    reg[F] = set_bit(reg[F], FN, 1);
    reg[F] = set_bit(reg[F], FH, 1);

    PC += 1;
    clock += 4;

    fprintf(stdout, "CPL\n");
}

// Switch carry flag
void CPU::ccf()
{
    bool carry;

    reg[F] = set_bit(reg[F], FN, 0);
    reg[F] = set_bit(reg[F], FH, 0);
    reg[F] = set_bit(reg[F], FC, get_bit(reg[F], FC) == 0);

    PC += 1;
    clock += 4;

    fprintf(stdout, "CCF\n");
}

void CPU::daa()
{
    //bool carry;

    // TODO

    /*reg[F] = set_bit(reg[F], FZ, == 0);
    reg[F] = set_bit(reg[F], FH, 0);
    reg[F] = set_bit(reg[F], FC, carry);*/

    PC += 1;
    clock += 4;

    fprintf(stdout, "DAA\n");
}

// Set carry flag
void CPU::scf()
{
    reg[F] = set_bit(reg[F], FN, 0);
    reg[F] = set_bit(reg[F], FH, 0);
    reg[F] = set_bit(reg[F], FC, 1);

    PC += 1;
    clock += 4;

    fprintf(stdout, "SCF\n");
}

void CPU::stop()
{
    // TODO

    PC += 2;
    clock += 4;

    fprintf(stdout, "STOP\n");
}

void CPU::halt()
{
    // TODO

    PC += 1;
    clock += 4;

    fprintf(stdout, "HALT\n");
}

void CPU::add()
{
    size_t ticks = 4;

    uint8_t opcode = mmu->get(PC);
    switch(opcode) {
    /* Add 16-bit registers to HL */
    case 0x09:
        add16(&reg[HL], &reg[BC]);
        ticks = 8;
        break;
    case 0x19:
        add16(&reg[HL], &reg[DE]);
        ticks = 8;
        break;
    case 0x29:
        add16(&reg[HL], &reg[HL]);
        ticks = 8;
        break;
    case 0x39:
        add16(&reg[HL], &reg[SP]);
        ticks = 8;
        break;
    }

    PC += 1;
    clock += ticks;
}

void CPU::inc()
{
    size_t ticks = 4;

    uint8_t opcode = mmu->get(PC);
    switch(opcode) {
    /* Inc 16-bit registers */
    case 0x03:
        inc16(&reg[BC]);
        ticks = 8;
        break;
    case 0x13:
        inc16(&reg[DE]);
        ticks = 8;
        break;
    case 0x23:
        inc16(&reg[HL]);
        ticks = 8;
        break;
    case 0x33:
        inc16((uint8_t*)&SP);
        ticks = 8;
        break;

    /* Inc 8-bit registers */
    case 0x04:
        inc8(&reg[B]);
        break;
    case 0x14:
        inc8(&reg[D]);
        break;
    case 0x24:
        inc8(&reg[H]);
        break;
    case 0x34:
        inc8((uint8_t*)mmu->at(reg[HL]));
        break;
    case 0x0C:
        inc8(&reg[C]);
        break;
    case 0x1C:
        inc8(&reg[E]);
        break;
    case 0x2C:
        inc8(&reg[L]);
        break;
    case 0x3C:
        inc8(&reg[A]);
        break;
    }

    PC += 1;
    clock += ticks;

    fprintf(stdout, "INC\n");
}

void CPU::dec()
{
    size_t ticks = 4;

    uint8_t opcode = mmu->get(PC);
    switch(opcode) {
    /* Dec 16-bit registers */
    case 0x0B:
        dec16(&reg[BC]);
        ticks = 8;
        break;
    case 0x1B:
        dec16(&reg[DE]);
        ticks = 8;
        break;
    case 0x2B:
        dec16(&reg[HL]);
        ticks = 8;
        break;
    case 0x3B:
        dec16((uint8_t*)&SP);
        ticks = 8;
        break;

    /* Dec 8-bit registers */
    case 0x05:
        dec8(&reg[B]);
        break;
    case 0x15:
        dec8(&reg[D]);
        break;
    case 0x25:
        dec8(&reg[H]);
        break;
    case 0x35:
        dec8((uint8_t*)mmu->at(reg[HL]));
        break;
    case 0x0D:
        dec8(&reg[C]);
        break;
    case 0x1D:
        dec8(&reg[E]);
        break;
    case 0x2D:
        dec8(&reg[L]);
        break;
    case 0x3D:
        dec8(&reg[A]);
        break;
    }

    PC += 1;
    clock += ticks;

    fprintf(stdout, "DEC\n");
}

void CPU::jr()
{
    int increment = 0;
    size_t ticks = 8;

    uint8_t opcode = mmu->get(PC);
    switch(opcode) {
    case 0x20:      // Jump if flag Z is not set
        if (get_bit(reg[F], FZ) == 0) {
            increment = mmu->get_signed(PC + 1);
            ticks = 12;
        }
        break;

    case 0x30:      // Jump if flag C is not set
        if (get_bit(reg[F], FC) == 0) {
            increment = mmu->get_signed(PC + 1);
            ticks = 12;
        }

    case 0x28:      // Jump if flag Z is set
        if (get_bit(reg[F], FZ) == 1) {
            increment = mmu->get_signed(PC + 1);
            ticks = 12;
        }
        break;

    case 0x38:      // Jump if flag C is set
        if (get_bit(reg[F], FC) == 1) {
            increment = mmu->get_signed(PC + 1);
            ticks = 12;
        }
        break;

    case 0x18:
        increment = mmu->get_signed(PC + 1);
        ticks = 12;
        break;
    }

    PC += 2;            // Instruction we just read
    PC += increment;    // Jump
    clock += ticks;

    fprintf(stdout, "JR %d\n", increment);
}

void CPU::ld()
{
    uint8_t opcode = mmu->get(PC);

    /* Generic cases */
    if (opcode >= 0x40 && opcode < 0x80) {
        uint8_t *l_address[] = {
            &reg[B],
            &reg[C],
            &reg[D],
            &reg[E],
            &reg[H],
            &reg[L],
            (uint8_t*) mmu->at(reg[HL]),
            &reg[A]
        };

        uint8_t *dst = l_address[(opcode - 0x40) % 8];
        uint8_t *src = l_address[(opcode - 0x40) / 8];

        size_t ticks = 4;
        if (opcode % 8 == 6) {
            ticks += 4;
        }

        return ld8(dst, src, 1, ticks);
    }

    uint16_t high, low, address;

    /* Less generic cases */
    switch (opcode) {
    /* Load 16-bit immediate to r16 */
    case 0x01:    // Loads 16-bit immediate to BC
        ld16(&reg[BC], (uint8_t*)mmu->at(PC + 1), 3, 12);
        break;

    case 0x11:    // Loads 16-bit immediate to DE
        ld16(&reg[DE], (uint8_t*)mmu->at(PC + 1), 3, 12);
        break;

    case 0x21:    // Loads 16-bit immediate to HL
        ld16(&reg[HL], (uint8_t*)mmu->at(PC + 1), 3, 12);
        break;

    case 0x31:    // Loads 16-bit immediate to SP
        ld16((uint8_t*)&SP, (uint8_t*)mmu->at(PC + 1), 3, 12);
        break;

    /* Load reg A into pointed address */
    case 0x02:    // Loads reg A to (BC)
        ld8(mmu->at(reg[BC]), &reg[A], 1, 8);
        break;

    case 0x12:    // Loads reg A to (DE)
        ld8(mmu->at(reg[DE]), &reg[A], 1, 8);
        break;

    case 0x22:    // Loads reg A to (HL), inc HL
        ld8(mmu->at(reg[HL]), &reg[A], 1, 8);
        inc16(&reg[HL]);
        break;

    case 0x32:    // Loads reg A to (HL), dec HL
        ld8(mmu->at(reg[HL]), &reg[A], 1, 8);
        dec16(&reg[HL]);
        break;

    /* Load 16-bit Sp to (immediate 16-bit) */
    case 0x08:
        high = mmu->get(PC + 1);
        low = mmu->get(PC + 2);
        address = (high << 4) + low;
        ld16((uint8_t*)mmu->at(address), (uint8_t*)&SP, 3, 20);
        break;

    /* Load 8-bit immediate to reg */
    case 0x06:    // Loads 8-bit immediate to B
        ld8(&reg[B], mmu->at(PC + 1), 2, 8);
        break;

    case 0x16:    // Loads 8-bit immediate to D
        ld8(&reg[D], mmu->at(PC + 1), 2, 8);
        break;

    case 0x26:    // Loads 8-bit immediate to H
        ld8(&reg[H], mmu->at(PC + 1), 2, 8);
        break;

    case 0x36:    // Loads 8-bit immediate to (HL)
        ld8(mmu->at(reg[HL]), mmu->at(PC + 1), 2, 12);
        break;

    /* Load pointed address into A */
    case 0x0A:    // Loads (BC) to reg A
        ld8(&reg[A], mmu->at(reg[BC]), 1, 8);
        break;

    case 0x1A:    // Loads (DE) to reg A
        ld8(&reg[A], mmu->at(reg[DE]), 1, 8);
        break;

    case 0x2A:    // Loads (HL) to reg A, inc HL
        ld8(&reg[A], mmu->at(reg[HL]), 1, 8);
        inc16(&reg[HL]);
        break;

    case 0x3A:    // Loads (HL) to reg A, dec HL
        ld8(&reg[A], mmu->at(reg[HL]), 1, 8);
        dec16(&reg[HL]);
        break;

    /* Load 8-bit immediate to reg */
    case 0x0E:    // Loads 8-bit immediate to C
        ld8(&reg[C], mmu->at(PC + 1), 2, 8);
        break;

    case 0x1E:    // Loads 8-bit immediate to E
        ld8(&reg[E], mmu->at(PC + 1), 2, 8);
        break;

    case 0x2E:    // Loads 8-bit immediate to L
        ld8(&reg[L], mmu->at(PC + 1), 2, 8);
        break;

    case 0x3E:    // Loads 8-bit immediate to A
        ld8(&reg[A], mmu->at(PC + 1), 2, 8);
        break;
    }
}

void CPU::rxa()
{
    uint8_t opcode = mmu->get(PC + 1);
    bool is_left = true;
    if (opcode == 0x0F || opcode == 0x1F) {
        is_left = false;
    }

    bool carry, old_carry = get_bit(reg[F], FC);
    reg[A] = rotate(reg[A], is_left, &carry);

    /* Through carry flag */
    if (opcode == 0x17 || opcode == 0x01F) {
        if (is_left) {
            reg[A] = set_bit(reg[A], 0, old_carry);
        } else {
            reg[A] = set_bit(reg[A], 7, old_carry);
        }
    }

    reg[F] = set_bit(reg[F], FZ, 0);
    reg[F] = set_bit(reg[F], FN, 0);
    reg[F] = set_bit(reg[F], FH, 0);
    reg[F] = set_bit(reg[F], FC, carry);

    PC += 1;
    clock += 4;

    fprintf(stdout, "RLCA\n");
}

void CPU::nop()
{
    PC += 1;
    clock += 4;

    fprintf(stdout, "NOP\n");
}

void CPU::prefix_CB()
{
    uint8_t *l_address[] = {
        &reg[B],
        &reg[C],
        &reg[D],
        &reg[E],
        &reg[H],
        &reg[L],
        (uint8_t*) mmu->at(reg[HL]),
        &reg[A]
    };

    uint8_t opcode = mmu->get(PC + 1);
    uint8_t *address = l_address[opcode % 8];

    size_t offset = (opcode / 8) % 8;
    bool left = (opcode % 16) < 8;
    bool carry, old_carry = get_bit(reg[F], FC);

    /* Rotate */
    if (opcode < 0x20) {
        *address = rotate(*address, left, &carry);

        /* Through carry flag */
        if (opcode >= 0x10) {
            if (left) {
                *address = set_bit(*address, 0, old_carry);
            } else {
                *address = set_bit(*address, 7, old_carry);
            }
        }

        reg[F] = set_bit(reg[F], FZ, *address == 0);
        reg[F] = set_bit(reg[F], FN, 0);
        reg[F] = set_bit(reg[F], FH, 0);
        reg[F] = set_bit(reg[F], FC, carry);
    }
    /* Shitf Left (SLA) */
    else if (opcode < 0x28) {
        shift(*address, true, &carry);

        reg[F] = set_bit(reg[F], FZ, *address == 0);
        reg[F] = set_bit(reg[F], FN, 0);
        reg[F] = set_bit(reg[F], FH, 0);
        reg[F] = set_bit(reg[F], FC, carry);
    }
    /* Shitf Right, keep b7 (SRA) */
    else if (opcode < 0x30) {
        bool b7 = get_bit(*address, 7);

        shift(*address, false, &carry);

        *address = set_bit(*address, 7, b7);

        reg[F] = set_bit(reg[F], FZ, *address == 0);
        reg[F] = set_bit(reg[F], FN, 0);
        reg[F] = set_bit(reg[F], FH, 0);
        reg[F] = set_bit(reg[F], FC, carry);
    }
    /* Swap */
    else if (opcode < 0x38) {
        *address = swap(*address);

        reg[F] = set_bit(reg[F], FZ, *address == 0);
        reg[F] = set_bit(reg[F], FN, 0);
        reg[F] = set_bit(reg[F], FH, 0);
        reg[F] = set_bit(reg[F], FC, 0);
    }
    /* Shitf Right (SLA) */
    else if (opcode < 0x40) {
        shift(*address, false, &carry);

        reg[F] = set_bit(reg[F], FZ, *address == 0);
        reg[F] = set_bit(reg[F], FN, 0);
        reg[F] = set_bit(reg[F], FH, 0);
        reg[F] = set_bit(reg[F], FC, carry);
    }
    /* Get Bit */
    else if (opcode < 0x80) {
        fprintf(stdout, "value=%02X%02X offset:%d bit:%d\n",
            *address,
            *(address + 1),
            offset,
            get_bit(*address, offset)
        );
        reg[F] = set_bit(reg[F], FZ, get_bit(*address, offset) == 0);
        reg[F] = set_bit(reg[F], FN, 0);
        reg[F] = set_bit(reg[F], FH, 1);
    }
    /* Set and Reset */
    else {
        *address = set_bit(*address, offset, opcode >= 0xC0);
    }

    size_t ticks = 8;
    /* Addressing (HL) takes 8 additional cycles */
    if (opcode % 8 == 6) {
        ticks += 8;
    }

    PC += 2;
    clock += ticks;

    fprintf(stdout, "Prefix CB\n");
}

void CPU::_xor()
{
    uint8_t opcode = mmu->get(PC);
    switch (opcode) {
    case 0xAF:    // XOR A
    default:
        reg[A] ^= reg[A];

        reg[F] = set_bit(reg[F], FZ, reg[A] == 0);
        reg[F] = set_bit(reg[F], FN, 0);
        reg[F] = set_bit(reg[F], FH, 0);
        reg[F] = set_bit(reg[F], FC, 0);

        PC += 1;
        clock += 4;

        fprintf(stdout, "XOR A\n");
    }
}
