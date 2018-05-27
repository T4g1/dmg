#include <stdio.h>
#include <string.h>
#include <unistd.h>     // DEBUG

#include "utils.h"
#include "log.h"

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

    l_callback[0x80] = 0;
    l_callback[0x81] = 0;
    l_callback[0x82] = 0;
    l_callback[0x83] = 0;
    l_callback[0x84] = 0;
    l_callback[0x85] = 0;
    l_callback[0x86] = 0;
    l_callback[0x87] = 0;
    l_callback[0x88] = 0;
    l_callback[0x89] = 0;
    l_callback[0x8A] = 0;
    l_callback[0x8B] = 0;
    l_callback[0x8C] = 0;
    l_callback[0x8D] = 0;
    l_callback[0x8E] = 0;
    l_callback[0x8F] = 0;

    l_callback[0x90] = 0;
    l_callback[0x91] = 0;
    l_callback[0x92] = 0;
    l_callback[0x93] = 0;
    l_callback[0x94] = 0;
    l_callback[0x95] = 0;
    l_callback[0x96] = 0;
    l_callback[0x97] = 0;
    l_callback[0x98] = 0;
    l_callback[0x99] = 0;
    l_callback[0x9A] = 0;
    l_callback[0x9B] = 0;
    l_callback[0x9C] = 0;
    l_callback[0x9D] = 0;
    l_callback[0x9E] = 0;
    l_callback[0x9F] = 0;

    l_callback[0xA0] = 0;
    l_callback[0xA1] = 0;
    l_callback[0xA2] = 0;
    l_callback[0xA3] = 0;
    l_callback[0xA4] = 0;
    l_callback[0xA5] = 0;
    l_callback[0xA6] = 0;
    l_callback[0xA7] = 0;
    l_callback[0xA8] = 0;
    l_callback[0xA9] = 0;
    l_callback[0xAA] = 0;
    l_callback[0xAB] = 0;
    l_callback[0xAC] = 0;
    l_callback[0xAD] = 0;
    l_callback[0xAE] = 0;
    l_callback[0xAF] = &_xor;

    l_callback[0xB0] = 0;
    l_callback[0xB1] = 0;
    l_callback[0xB2] = 0;
    l_callback[0xB3] = 0;
    l_callback[0xB4] = 0;
    l_callback[0xB5] = 0;
    l_callback[0xB6] = 0;
    l_callback[0xB7] = 0;
    l_callback[0xB8] = 0;
    l_callback[0xB9] = 0;
    l_callback[0xBA] = 0;
    l_callback[0xBB] = 0;
    l_callback[0xBC] = 0;
    l_callback[0xBD] = 0;
    l_callback[0xBE] = 0;
    l_callback[0xBF] = 0;

    l_callback[0xC0] = &ret;
    l_callback[0xC1] = &pop;
    l_callback[0xC2] = 0;
    l_callback[0xC3] = 0;
    l_callback[0xC4] = &call;
    l_callback[0xC5] = &push;
    l_callback[0xC6] = 0;
    l_callback[0xC7] = 0;
    l_callback[0xC8] = &ret;
    l_callback[0xC9] = &ret;
    l_callback[0xCA] = 0;
    l_callback[0xCB] = &prefix_CB;
    l_callback[0xCC] = &call;
    l_callback[0xCD] = &call;
    l_callback[0xCE] = 0;
    l_callback[0xCF] = 0;

    l_callback[0xD0] = &ret;
    l_callback[0xD1] = &pop;
    l_callback[0xD2] = 0;
    l_callback[0xD3] = NULL;
    l_callback[0xD4] = &call;
    l_callback[0xD5] = &push;
    l_callback[0xD6] = 0;
    l_callback[0xD7] = 0;
    l_callback[0xD8] = &ret;
    l_callback[0xD9] = &ret;
    l_callback[0xDA] = 0;
    l_callback[0xDB] = NULL;
    l_callback[0xDC] = &call;
    l_callback[0xDD] = NULL;
    l_callback[0xDE] = 0;
    l_callback[0xDF] = 0;

    l_callback[0xE0] = &ld;
    l_callback[0xE1] = &pop;
    l_callback[0xE2] = &ld;
    l_callback[0xE3] = NULL;
    l_callback[0xE4] = NULL;
    l_callback[0xE5] = &push;
    l_callback[0xE6] = 0;
    l_callback[0xE7] = 0;
    l_callback[0xE8] = 0;
    l_callback[0xE9] = 0;
    l_callback[0xEA] = &ld;
    l_callback[0xEB] = NULL;
    l_callback[0xEC] = NULL;
    l_callback[0xED] = NULL;
    l_callback[0xEE] = 0;
    l_callback[0xEF] = 0;

    l_callback[0xF0] = &ld;
    l_callback[0xF1] = &pop;
    l_callback[0xF2] = &ld;
    l_callback[0xF3] = 0;
    l_callback[0xF4] = NULL;
    l_callback[0xF5] = &push;
    l_callback[0xF6] = 0;
    l_callback[0xF7] = 0;
    l_callback[0xF8] = &ld;
    l_callback[0xF9] = &ld;
    l_callback[0xFA] = &ld;
    l_callback[0xFB] = 0;
    l_callback[0xFC] = NULL;
    l_callback[0xFD] = NULL;
    l_callback[0xFE] = 0;
    l_callback[0xFF] = 0;
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

    reg[SP] = 0xFF;
    reg[SP+1] = 0xFE;

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

    debug("PC: %08X\tClock: %d\tOpcode: %02X\n", PC, (int)clock, opcode);

    // DEBUG: Should crash if NULL
    if (l_callback[opcode] == NULL) {
        debug("Not Implemented!\n");
        PC += 1; // Just in case
        return;
    }

    (*this.*l_callback[opcode])();

    // DEBUG
    /*if (PC == 0x0027) {
        mmu->dump(0x0000, 0x0100);
        sleep(1);
    }*/
    if (PC >= 0x00A3) {
        sleep(1);
    }
}

/**
 * @brief Gives the 16-bit register wanted
 * @details Ensure H-L byte order
 */
uint16_t CPU::reg16(size_t i)
{
    uint16_t high = (uint16_t)reg[i];
    uint16_t low = (uint16_t)reg[i + 1];

    return (high << 8) + low;
}

void CPU::ld8(void *dst, void* src, size_t size, size_t ticks)
{
    memcpy(dst, src, sizeof(uint8_t));
    PC += size;
    clock += ticks;

    debug("LD reg\n");
}

/**
 * @brief Loads 16-bit value and invert order
 */
void CPU::ld16(uint8_t *dst, uint8_t* src, size_t size, size_t ticks)
{
    // Invert LOW and HIGH bytes
    memcpy(dst + 1, src, sizeof(uint8_t));
    memcpy(dst, src + 1, sizeof(uint8_t));

    PC += size;
    clock += ticks;

    debug("LD r16\n");
}

void CPU::inc8(uint8_t *address)
{
    *address = *address + 1;

    reg[F] = set_bit(reg[F], FZ, *address == 0);
    reg[F] = set_bit(reg[F], FN, 0);
    reg[F] = set_bit(reg[F], FH, (*address & 0x0F) == 0x00); // Went from 0xFF to 0x00
}

void CPU::dec8(uint8_t *address)
{
    *address = *address - 1;

    reg[F] = set_bit(reg[F], FZ, *address == 0);
    reg[F] = set_bit(reg[F], FN, 1);
    reg[F] = set_bit(reg[F], FH, (*address & 0x0F) == 0x0F); // Went from 0x00 to 0xFF
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
    }
    *dl = (uint8_t)(tmp & 0xFF);

    int idh = (int)*dh;
    int ish = (int)*sh;
    tmp = idh + ish + half_carry;
    if (tmp > 255) {
        carry = true;
    }
    *dh = (uint8_t)(tmp & 0xFF);

    reg[F] = set_bit(reg[F], FN, 0);
    reg[F] = set_bit(reg[F], FH, half_carry == 1);
    reg[F] = set_bit(reg[F], FC, carry = 1);

    debug("ADD r16\n");
}

void CPU::push()
{
    uint8_t opcode = mmu->get(PC);
    uint8_t value;
    if (opcode == 0xC5) {
        value = reg16(BC);
    }
    if (opcode == 0xD5) {
        value = reg16(DE);
    }
    if (opcode == 0xE5) {
        value = reg16(HL);
    }
    if (opcode == 0xF5) {
        value = reg16(AF);
    }

    uint8_t high = (uint8_t)((value & 0xFF00) >> 4);
    uint8_t low = (uint8_t)(value & 0x00FF);

    mmu->set(reg16(SP), high);
    dec16(&reg[SP]);
    mmu->set(reg16(SP), low);
    dec16(&reg[SP]);

    PC += 1;
    clock += 16;

    debug("PUSH\n");
}

void CPU::pop()
{
    uint8_t opcode = mmu->get(PC);
    uint8_t *high;
    if (opcode == 0xC1) {
        high = &reg[BC];
    }
    if (opcode == 0xD1) {
        high = &reg[DE];
    }
    if (opcode == 0xE1) {
        high = &reg[HL];
    }
    if (opcode == 0xF1) {
        high = &reg[AF];
    }

    uint8_t *low = high + 1;

    *low = mmu->get(reg16(SP));
    inc16(&reg[SP]);
    *high = mmu->get(reg16(SP));
    inc16(&reg[SP]);

    PC += 1;
    clock += 12;

    debug("POP\n");
}

void CPU::call()
{
    uint8_t ticks = 24;

    uint8_t opcode = mmu->get(PC);
    uint16_t address = mmu->get16(PC + 1);

    PC += 3;

    uint8_t high = (uint8_t)((PC & 0xFF00) >> 4);
    uint8_t low = (uint8_t)(PC & 0x00FF);

    bool do_call = true;
    /* Call if not Z */
    if ((opcode == 0xC4 && get_bit(reg[F], FZ) == 1) ||
    /* Call if not C */
        (opcode == 0xD4 && get_bit(reg[F], FC) == 1) ||
    /* Call if Z */
        (opcode == 0xCC && get_bit(reg[F], FZ) == 0) ||
    /* Call if C */
        (opcode == 0xDC && get_bit(reg[F], FC) == 0)) {
        ticks = 12;
        do_call = false;
    }

    if (do_call) {
        mmu->set(reg16(SP), high);
        dec16(&reg[SP]);
        mmu->set(reg16(SP), low);
        dec16(&reg[SP]);

        PC = address;
    }

    clock += ticks;

    debug("CALL %04X\n", address);
}

void CPU::ret()
{
    uint8_t ticks = 16;

    uint8_t opcode = mmu->get(PC);

    PC += 1;

    bool do_ret = true;
    /* Ret if not Z */
    if ((opcode == 0xC0 && get_bit(reg[F], FZ) == 1) ||
    /* Ret if not C */
        (opcode == 0xD0 && get_bit(reg[F], FC) == 1) ||
    /* Ret if Z */
        (opcode == 0xC8 && get_bit(reg[F], FZ) == 0) ||
    /* Ret if C */
        (opcode == 0xD8 && get_bit(reg[F], FC) == 0)) {
        ticks = 8;
        do_ret = false;
    } else {
        ticks = 20;
    }

    /* RETI */
    if (opcode == 0xD9) {
        // TODO enable interuptions
    }

    if (do_ret) {
        PC = mmu->get16(reg16(SP));
        inc16(&reg[SP]);
        inc16(&reg[SP]);
    }

    clock += ticks;

    debug("RET\n");
}

// Invert reg A
void CPU::cpl()
{
    reg[A] = ~reg[A];

    reg[F] = set_bit(reg[F], FN, 1);
    reg[F] = set_bit(reg[F], FH, 1);

    PC += 1;
    clock += 4;

    debug("CPL\n");
}

// Switch carry flag
void CPU::ccf()
{
    reg[F] = set_bit(reg[F], FN, 0);
    reg[F] = set_bit(reg[F], FH, 0);
    reg[F] = set_bit(reg[F], FC, get_bit(reg[F], FC) == 0);

    PC += 1;
    clock += 4;

    debug("CCF\n");
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

    debug("DAA\n");
}

// Set carry flag
void CPU::scf()
{
    reg[F] = set_bit(reg[F], FN, 0);
    reg[F] = set_bit(reg[F], FH, 0);
    reg[F] = set_bit(reg[F], FC, 1);

    PC += 1;
    clock += 4;

    debug("SCF\n");
}

void CPU::stop()
{
    // TODO

    PC += 2;
    clock += 4;

    debug("STOP\n");
}

void CPU::halt()
{
    // TODO

    PC += 1;
    clock += 4;

    debug("HALT\n");
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
        inc16(&reg[SP]);
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
        inc8((uint8_t*)mmu->at(reg16(HL)));
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

    debug("INC\n");
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
        dec16(&reg[SP]);
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
        dec8((uint8_t*)mmu->at(reg16(HL)));
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

    debug("DEC\n");
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
        break;

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

    debug("JR %d\n", increment);
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
            (uint8_t*) mmu->at(reg16(HL)),
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

    uint16_t address;
    uint8_t value[2] = {0};

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
        ld16(&reg[SP], (uint8_t*)mmu->at(PC + 1), 3, 12);
        break;

    /* Load reg A into pointed address */
    case 0x02:    // Loads reg A to (BC)
        ld8(mmu->at(reg16(BC)), &reg[A], 1, 8);
        break;

    case 0x12:    // Loads reg A to (DE)
        ld8(mmu->at(reg16(DE)), &reg[A], 1, 8);
        break;

    case 0x22:    // Loads reg A to (HL), inc HL
        ld8(mmu->at(reg16(HL)), &reg[A], 1, 8);
        inc16(&reg[HL]);
        break;

    case 0x32:    // Loads reg A to (HL), dec HL
        ld8(mmu->at(reg16(HL)), &reg[A], 1, 8);
        dec16(&reg[HL]);
        break;

    /* Load 16-bit Sp to (immediate 16-bit) */
    case 0x08:
        address = mmu->get16(PC + 1);
        ld16((uint8_t*)mmu->at(address), &reg[SP], 3, 20);
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
        ld8(mmu->at(reg16(HL)), mmu->at(PC + 1), 2, 12);
        break;

    /* Load pointed address into A */
    case 0x0A:    // Loads (BC) to reg A
        ld8(&reg[A], mmu->at(reg16(BC)), 1, 8);
        break;

    case 0x1A:    // Loads (DE) to reg A
        ld8(&reg[A], mmu->at(reg16(DE)), 1, 8);
        break;

    case 0x2A:    // Loads (HL) to reg A, inc HL
        ld8(&reg[A], mmu->at(reg16(HL)), 1, 8);
        inc16(&reg[HL]);
        break;

    case 0x3A:    // Loads (HL) to reg A, dec HL
        ld8(&reg[A], mmu->at(reg16(HL)), 1, 8);
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

    /* Loads from/to 8-bit address */
    case 0xE0:      // Loads reg A to address immediate
        address = 0xFF00 + mmu->get(PC + 1);
        ld8(mmu->at(address), &reg[A], 2, 12);
        break;

    case 0xF0:      // Loads from address immediate to reg A
        address = 0xFF00 + mmu->get(PC + 1);
        ld8(&reg[A], mmu->at(address), 2, 12);
        break;

    case 0xE2:      // Loads from addres reg C to reg A
        ld8(mmu->at(0xFF00 + reg[C]), &reg[A], 2, 8);
        break;

    case 0xF2:      // Loads from reg A to addres reg C
        ld8(&reg[A], mmu->at(0xFF00 + reg[C]), 2, 8);
        break;

    /* Loads from/to 16-bit address */
    case 0xEA:      // Loads reg A to address immediate
        address = mmu->get16(PC + 1);
        ld8(mmu->at(address), &reg[A], 3, 16);
        break;

    case 0xFA:      // Loads from address immediate to reg A
        address = mmu->get16(PC + 1);
        ld8(&reg[A], mmu->at(address), 3, 16);
        break;

    case 0xF8:      // Loads SP+r8 to HL
        memcpy(&reg[HL], &reg[SP], sizeof(uint16_t));
        value[0] = 0x00;
        value[1] = mmu->get(PC+1);
        add16(&reg[HL], value);

        PC += 2;
        clock += 12;

        reg[F] = set_bit(reg[F], FZ, 0);
        break;

    case 0xF9:      // Loads HL content to SP
        memcpy(&reg[SP], &reg[HL], sizeof(uint16_t));

        PC += 1;
        clock += 8;
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

    debug("RLCA\n");
}

void CPU::nop()
{
    PC += 1;
    clock += 4;

    debug("NOP\n");
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
        (uint8_t*) mmu->at(reg16(HL)),
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

    debug("Prefix CB\n");
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

        debug("XOR A\n");
    }
}
