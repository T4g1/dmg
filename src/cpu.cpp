#include <stdio.h>
#include <string.h>
#include <unistd.h>     // DEBUG

#include "utils.h"
#include "log.h"

#include "cpu.h"

//#define debug_cpu info
#define debug_cpu(...)

CPU::CPU(MMU *mmu) : mmu(mmu)
{
    PC = 0;
    clock = 0;

    // CPU opcode assingation
    l_callback[0x00] = &CPU::nop;
    l_callback[0x01] = &CPU::ld;
    l_callback[0x02] = &CPU::ld;
    l_callback[0x03] = &CPU::inc;
    l_callback[0x04] = &CPU::inc;
    l_callback[0x05] = &CPU::dec;
    l_callback[0x06] = &CPU::ld;
    l_callback[0x07] = &CPU::rxa;
    l_callback[0x08] = &CPU::ld;
    l_callback[0x09] = &CPU::add;
    l_callback[0x0A] = &CPU::ld;
    l_callback[0x0B] = &CPU::dec;
    l_callback[0x0C] = &CPU::inc;
    l_callback[0x0D] = &CPU::dec;
    l_callback[0x0E] = &CPU::ld;
    l_callback[0x0F] = &CPU::rxa;

    l_callback[0x10] = &CPU::stop;
    l_callback[0x11] = &CPU::ld;
    l_callback[0x12] = &CPU::ld;
    l_callback[0x13] = &CPU::inc;
    l_callback[0x14] = &CPU::inc;
    l_callback[0x15] = &CPU::dec;
    l_callback[0x16] = &CPU::ld;
    l_callback[0x17] = &CPU::rxa;
    l_callback[0x18] = &CPU::jr;
    l_callback[0x19] = &CPU::add;
    l_callback[0x1A] = &CPU::ld;
    l_callback[0x1B] = &CPU::dec;
    l_callback[0x1C] = &CPU::inc;
    l_callback[0x1D] = &CPU::dec;
    l_callback[0x1E] = &CPU::ld;
    l_callback[0x1F] = &CPU::rxa;

    l_callback[0x20] = &CPU::jr;
    l_callback[0x21] = &CPU::ld;
    l_callback[0x22] = &CPU::ld;
    l_callback[0x23] = &CPU::inc;
    l_callback[0x24] = &CPU::inc;
    l_callback[0x25] = &CPU::dec;
    l_callback[0x26] = &CPU::ld;
    l_callback[0x27] = &CPU::daa;
    l_callback[0x28] = &CPU::jr;
    l_callback[0x29] = &CPU::add;
    l_callback[0x2A] = &CPU::ld;
    l_callback[0x2B] = &CPU::dec;
    l_callback[0x2C] = &CPU::inc;
    l_callback[0x2D] = &CPU::dec;
    l_callback[0x2E] = &CPU::ld;
    l_callback[0x2F] = &CPU::cpl;

    l_callback[0x30] = &CPU::jr;
    l_callback[0x31] = &CPU::ld;
    l_callback[0x32] = &CPU::ld;
    l_callback[0x33] = &CPU::inc;
    l_callback[0x34] = &CPU::inc;
    l_callback[0x35] = &CPU::dec;
    l_callback[0x36] = &CPU::ld;
    l_callback[0x37] = &CPU::scf;
    l_callback[0x38] = &CPU::jr;
    l_callback[0x39] = &CPU::add;
    l_callback[0x3A] = &CPU::ld;
    l_callback[0x3B] = &CPU::dec;
    l_callback[0x3C] = &CPU::inc;
    l_callback[0x3D] = &CPU::dec;
    l_callback[0x3E] = &CPU::ld;
    l_callback[0x3F] = &CPU::ccf;

    l_callback[0x40] = &CPU::ld;
    l_callback[0x41] = &CPU::ld;
    l_callback[0x42] = &CPU::ld;
    l_callback[0x43] = &CPU::ld;
    l_callback[0x44] = &CPU::ld;
    l_callback[0x45] = &CPU::ld;
    l_callback[0x46] = &CPU::ld;
    l_callback[0x47] = &CPU::ld;
    l_callback[0x48] = &CPU::ld;
    l_callback[0x49] = &CPU::ld;
    l_callback[0x4A] = &CPU::ld;
    l_callback[0x4B] = &CPU::ld;
    l_callback[0x4C] = &CPU::ld;
    l_callback[0x4D] = &CPU::ld;
    l_callback[0x4E] = &CPU::ld;
    l_callback[0x4F] = &CPU::ld;

    l_callback[0x50] = &CPU::ld;
    l_callback[0x51] = &CPU::ld;
    l_callback[0x52] = &CPU::ld;
    l_callback[0x53] = &CPU::ld;
    l_callback[0x54] = &CPU::ld;
    l_callback[0x55] = &CPU::ld;
    l_callback[0x56] = &CPU::ld;
    l_callback[0x57] = &CPU::ld;
    l_callback[0x58] = &CPU::ld;
    l_callback[0x59] = &CPU::ld;
    l_callback[0x5A] = &CPU::ld;
    l_callback[0x5B] = &CPU::ld;
    l_callback[0x5C] = &CPU::ld;
    l_callback[0x5D] = &CPU::ld;
    l_callback[0x5E] = &CPU::ld;
    l_callback[0x5F] = &CPU::ld;

    l_callback[0x60] = &CPU::ld;
    l_callback[0x61] = &CPU::ld;
    l_callback[0x62] = &CPU::ld;
    l_callback[0x63] = &CPU::ld;
    l_callback[0x64] = &CPU::ld;
    l_callback[0x65] = &CPU::ld;
    l_callback[0x66] = &CPU::ld;
    l_callback[0x67] = &CPU::ld;
    l_callback[0x68] = &CPU::ld;
    l_callback[0x69] = &CPU::ld;
    l_callback[0x6A] = &CPU::ld;
    l_callback[0x6B] = &CPU::ld;
    l_callback[0x6C] = &CPU::ld;
    l_callback[0x6D] = &CPU::ld;
    l_callback[0x6E] = &CPU::ld;
    l_callback[0x6F] = &CPU::ld;

    l_callback[0x70] = &CPU::ld;
    l_callback[0x71] = &CPU::ld;
    l_callback[0x72] = &CPU::ld;
    l_callback[0x73] = &CPU::ld;
    l_callback[0x74] = &CPU::ld;
    l_callback[0x75] = &CPU::ld;
    l_callback[0x76] = &CPU::halt;
    l_callback[0x77] = &CPU::ld;
    l_callback[0x78] = &CPU::ld;
    l_callback[0x79] = &CPU::ld;
    l_callback[0x7A] = &CPU::ld;
    l_callback[0x7B] = &CPU::ld;
    l_callback[0x7C] = &CPU::ld;
    l_callback[0x7D] = &CPU::ld;
    l_callback[0x7E] = &CPU::ld;
    l_callback[0x7F] = &CPU::ld;

    l_callback[0x80] = &CPU::add;
    l_callback[0x81] = &CPU::add;
    l_callback[0x82] = &CPU::add;
    l_callback[0x83] = &CPU::add;
    l_callback[0x84] = &CPU::add;
    l_callback[0x85] = &CPU::add;
    l_callback[0x86] = &CPU::add;
    l_callback[0x87] = &CPU::add;
    l_callback[0x88] = &CPU::add;
    l_callback[0x89] = &CPU::add;
    l_callback[0x8A] = &CPU::add;
    l_callback[0x8B] = &CPU::add;
    l_callback[0x8C] = &CPU::add;
    l_callback[0x8D] = &CPU::add;
    l_callback[0x8E] = &CPU::add;
    l_callback[0x8F] = &CPU::add;

    l_callback[0x90] = &CPU::sub;
    l_callback[0x91] = &CPU::sub;
    l_callback[0x92] = &CPU::sub;
    l_callback[0x93] = &CPU::sub;
    l_callback[0x94] = &CPU::sub;
    l_callback[0x95] = &CPU::sub;
    l_callback[0x96] = &CPU::sub;
    l_callback[0x97] = &CPU::sub;
    l_callback[0x98] = &CPU::sub;
    l_callback[0x99] = &CPU::sub;
    l_callback[0x9A] = &CPU::sub;
    l_callback[0x9B] = &CPU::sub;
    l_callback[0x9C] = &CPU::sub;
    l_callback[0x9D] = &CPU::sub;
    l_callback[0x9E] = &CPU::sub;
    l_callback[0x9F] = &CPU::sub;

    l_callback[0xA0] = &CPU::or_xor_and_cp;
    l_callback[0xA1] = &CPU::or_xor_and_cp;
    l_callback[0xA2] = &CPU::or_xor_and_cp;
    l_callback[0xA3] = &CPU::or_xor_and_cp;
    l_callback[0xA4] = &CPU::or_xor_and_cp;
    l_callback[0xA5] = &CPU::or_xor_and_cp;
    l_callback[0xA6] = &CPU::or_xor_and_cp;
    l_callback[0xA7] = &CPU::or_xor_and_cp;
    l_callback[0xA8] = &CPU::or_xor_and_cp;
    l_callback[0xA9] = &CPU::or_xor_and_cp;
    l_callback[0xAA] = &CPU::or_xor_and_cp;
    l_callback[0xAB] = &CPU::or_xor_and_cp;
    l_callback[0xAC] = &CPU::or_xor_and_cp;
    l_callback[0xAD] = &CPU::or_xor_and_cp;
    l_callback[0xAE] = &CPU::or_xor_and_cp;
    l_callback[0xAF] = &CPU::or_xor_and_cp;

    l_callback[0xB0] = &CPU::or_xor_and_cp;
    l_callback[0xB1] = &CPU::or_xor_and_cp;
    l_callback[0xB2] = &CPU::or_xor_and_cp;
    l_callback[0xB3] = &CPU::or_xor_and_cp;
    l_callback[0xB4] = &CPU::or_xor_and_cp;
    l_callback[0xB5] = &CPU::or_xor_and_cp;
    l_callback[0xB6] = &CPU::or_xor_and_cp;
    l_callback[0xB7] = &CPU::or_xor_and_cp;
    l_callback[0xB8] = &CPU::or_xor_and_cp;
    l_callback[0xB9] = &CPU::or_xor_and_cp;
    l_callback[0xBA] = &CPU::or_xor_and_cp;
    l_callback[0xBB] = &CPU::or_xor_and_cp;
    l_callback[0xBC] = &CPU::or_xor_and_cp;
    l_callback[0xBD] = &CPU::or_xor_and_cp;
    l_callback[0xBE] = &CPU::or_xor_and_cp;
    l_callback[0xBF] = &CPU::or_xor_and_cp;

    l_callback[0xC0] = &CPU::ret;
    l_callback[0xC1] = &CPU::pop;
    l_callback[0xC2] = &CPU::jp;
    l_callback[0xC3] = &CPU::jp;
    l_callback[0xC4] = &CPU::call;
    l_callback[0xC5] = &CPU::push;
    l_callback[0xC6] = &CPU::add;
    l_callback[0xC7] = &CPU::rst;
    l_callback[0xC8] = &CPU::ret;
    l_callback[0xC9] = &CPU::ret;
    l_callback[0xCA] = &CPU::jp;
    l_callback[0xCB] = &CPU::prefix_CB;
    l_callback[0xCC] = &CPU::call;
    l_callback[0xCD] = &CPU::call;
    l_callback[0xCE] = &CPU::add;
    l_callback[0xCF] = &CPU::rst;

    l_callback[0xD0] = &CPU::ret;
    l_callback[0xD1] = &CPU::pop;
    l_callback[0xD2] = &CPU::jp;
    l_callback[0xD3] = NULL;
    l_callback[0xD4] = &CPU::call;
    l_callback[0xD5] = &CPU::push;
    l_callback[0xD6] = &CPU::sub;
    l_callback[0xD7] = &CPU::rst;
    l_callback[0xD8] = &CPU::ret;
    l_callback[0xD9] = &CPU::ret;
    l_callback[0xDA] = &CPU::jp;
    l_callback[0xDB] = NULL;
    l_callback[0xDC] = &CPU::call;
    l_callback[0xDD] = NULL;
    l_callback[0xDE] = &CPU::sub;
    l_callback[0xDF] = &CPU::rst;

    l_callback[0xE0] = &CPU::ld;
    l_callback[0xE1] = &CPU::pop;
    l_callback[0xE2] = &CPU::ld;
    l_callback[0xE3] = NULL;
    l_callback[0xE4] = NULL;
    l_callback[0xE5] = &CPU::push;
    l_callback[0xE6] = &CPU::or_xor_and_cp;
    l_callback[0xE7] = &CPU::rst;
    l_callback[0xE8] = &CPU::add;
    l_callback[0xE9] = &CPU::jp_hl;
    l_callback[0xEA] = &CPU::ld;
    l_callback[0xEB] = NULL;
    l_callback[0xEC] = NULL;
    l_callback[0xED] = NULL;
    l_callback[0xEE] = &CPU::or_xor_and_cp;
    l_callback[0xEF] = &CPU::rst;

    l_callback[0xF0] = &CPU::ld;
    l_callback[0xF1] = &CPU::pop;
    l_callback[0xF2] = &CPU::ld;
    l_callback[0xF3] = &CPU::di;
    l_callback[0xF4] = NULL;
    l_callback[0xF5] = &CPU::push;
    l_callback[0xF6] = &CPU::or_xor_and_cp;
    l_callback[0xF7] = &CPU::rst;
    l_callback[0xF8] = &CPU::ld;
    l_callback[0xF9] = &CPU::ld;
    l_callback[0xFA] = &CPU::ld;
    l_callback[0xFB] = &CPU::ei;
    l_callback[0xFC] = NULL;
    l_callback[0xFD] = NULL;
    l_callback[0xFE] = &CPU::or_xor_and_cp;
    l_callback[0xFF] = &CPU::rst;
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
    mmu->set(0xFF24, 0x77);     //<! NR50
    mmu->set(0xFF25, 0xF3);     //<! NR51
    mmu->set(0xFF26, 0xF1);     //<! NR52
    mmu->set(LCDC, 0x91);       //<! LCDC
    mmu->set(SCY, 0x00);        //<! SCY
    mmu->set(SCX, 0x00);        //<! SCX
    mmu->set(LYC, 0x00);        //<! LYC
    mmu->set(0xFF47, 0xFC);     //<! BGP
    mmu->set(0xFF48, 0xFF);     //<! OBP0
    mmu->set(0xFF49, 0xFF);     //<! OBP1
    mmu->set(0xFF4A, 0x00);     //<! WY
    mmu->set(0xFF4B, 0x00);     //<! WX
    mmu->set(0xFFFF, 0x00);     //<! IE

    IME = true;
}

/**
 * @brief      Execute the opcode pointed by PC
 * @return     false in case of crash
 */
bool CPU::step()
{
    uint8_t opcode = mmu->get(PC);

    // DEBUG
    if (mmu->is_booted()) {
        info("PC: 0x%04X\tClock: %d\tOpcode: 0x%02X\n", PC, (int)clock, opcode);
    }

    // Some instructions are not meant to do anything, crash the system
    if (l_callback[opcode] == NULL) {
        error("Not Implemented PC: 0x%04X\tOpcode: 0x%02X\n", PC, opcode);
        PC += 1; // Just in case

        // TODO: Crash?
        return false;
    }

    (*this.*l_callback[opcode])();

    return true;
}

/**
 * @brief      Displays register values
 */
void CPU::display_registers()
{
    info("---------------------------[ CPU ]---------------------------\n");
    info("PC: 0x%04X\n", PC);
    info("-------------------------------------------------------------\n");
    info("A: 0x%02X F: 0x%02X\n", reg[A], reg[F]);
    info("B: 0x%02X C: 0x%02X\n", reg[B], reg[C]);
    info("D: 0x%02X E: 0x%02X\n", reg[D], reg[E]);
    info("H: 0x%02X L: 0x%02X\n", reg[H], reg[L]);
    info("-------------------------------------------------------------\n");
    info("AF: 0x%04X\n", reg16(AF));
    info("BC: 0x%04X\n", reg16(BC));
    info("DE: 0x%04X\n", reg16(DE));
    info("HL: 0x%04X\n", reg16(HL));
    info("SP: 0x%04X\n", reg16(SP));
}

/**
 * @brief      Give the value of the desired flag
 * @param[in]  flag  The flag index
 * @return     The flag value (True if it is set)
 */
bool CPU::get_flag(size_t flag)
{
    return get_bit(reg[F], flag);
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

void CPU::ld8_mmu(uint16_t address, const uint8_t* src, size_t size, size_t ticks)
{
    mmu->set(address, *(uint8_t*) src);
    PC += size;
    clock += ticks;

    debug_cpu("LD reg\n");
}

void CPU::ld8(uint8_t *dst, const uint8_t* src, size_t size, size_t ticks)
{
    memcpy(dst, src, sizeof(uint8_t));
    PC += size;
    clock += ticks;

    debug_cpu("LD reg\n");
}

/**
 * @brief Loads 16-bit value and invert order
 */
void CPU::ld16(uint8_t *dst, const uint8_t* src, size_t size, size_t ticks)
{
    // Invert LOW and HIGH bytes
    memcpy(dst + 1, src, sizeof(uint8_t));
    memcpy(dst, src + 1, sizeof(uint8_t));

    PC += size;
    clock += ticks;
}

void CPU::inc8(uint8_t *address)
{
    *address = *address + 1;

    reg[F] = set_bit(reg[F], FZ, *address == 0);
    reg[F] = set_bit(reg[F], FN, 0);
    reg[F] = set_bit(reg[F], FH, (*address & 0x0F) == 0x00); // Went from 0xXF to 0xX0
}

void CPU::dec8(uint8_t *address)
{
    *address = *address - 1;

    reg[F] = set_bit(reg[F], FZ, *address == 0);
    reg[F] = set_bit(reg[F], FN, 1);
    reg[F] = set_bit(reg[F], FH, (*address & 0x0F) == 0x0F); // Went from 0xX0 to 0xXF
}

/**
 * @brief      Handles ADD and ADC
 */
void CPU::add8()
{
    const uint8_t *l_address[] = {
        &reg[B],
        &reg[C],
        &reg[D],
        &reg[E],
        &reg[H],
        &reg[L],
        (const uint8_t*) mmu->at(reg16(HL)),
        &reg[A]
    };

    uint8_t opcode = mmu->get(PC);
    size_t target_index = opcode % 8;
    const uint8_t *target = l_address[target_index];

    // ADD/ADC d8
    if (opcode > 0xC0) {
        target = (const uint8_t*) mmu->at(PC + 1);
        PC += 2;
        clock += 8;
    }
    // (HL) case
    else if (target_index == 6) {
        PC += 1;
        clock += 8;
    } else {
        PC += 1;
        clock += 4;
    }

    uint16_t value = *target;

    // ADC
    if (opcode >= 0x88 && opcode != 0xC6) {
        value += get_bit(reg[F], FC);

        debug_cpu("ADC\n");
    } else {
        debug_cpu("ADD\n");
    }

    uint16_t result = reg[A] + value;

    // FH set when adding carry flag
    bool half_carry = (*target & 0xF0) < (value & 0xFFF0);

    half_carry |= (reg[A] & 0x0F) + (value & 0x0F) > 0x0F;

    reg[A] = (uint8_t) result;

    reg[F] = set_bit(reg[F], FZ, reg[A] == 0);
    reg[F] = set_bit(reg[F], FN, 0);
    reg[F] = set_bit(reg[F], FH, half_carry);
    reg[F] = set_bit(reg[F], FC, result > 0x00FF);  // Overflow
}

void CPU::add16(uint8_t *dst, const uint8_t *src)
{
    debug_cpu("ADD 0x%04X to 0x%04X\n", *(uint16_t*) dst, *(uint16_t*) src);
    bool half_carry = false, carry = false;

    uint8_t *dh = dst;
    const uint8_t *sh = src;
    uint8_t *dl = dst + 1;
    const uint8_t *sl = src + 1;

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

    debug_cpu("ADD r16\n");
}

void CPU::push()
{
    uint8_t opcode = mmu->get(PC);
    uint16_t value;
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

    uint8_t high = (uint8_t)((value & 0xFF00) >> 8);
    uint8_t low = (uint8_t)(value & 0x00FF);

    dec16(&reg[SP]);
    mmu->set(reg16(SP), high);
    dec16(&reg[SP]);
    mmu->set(reg16(SP), low);

    PC += 1;
    clock += 16;

    debug_cpu("PUSH\n");
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

    debug_cpu("POP\n");
}

void CPU::jp()
{
    uint8_t ticks = 16;

    uint8_t opcode = mmu->get(PC);
    uint16_t address = mmu->get16(PC + 1);

    bool jump = true;
    /* Jump if not Z */
    if ((opcode == 0xC2 && get_bit(reg[F], FZ) == 1) ||
    /* Jump if not C */
        (opcode == 0xD2 && get_bit(reg[F], FC) == 1) ||
    /* Jump if Z */
        (opcode == 0xCA && get_bit(reg[F], FZ) == 0) ||
    /* Jump if C */
        (opcode == 0xDA && get_bit(reg[F], FC) == 0)) {
        ticks = 12;
        jump = false;
    }

    if (jump) {
        PC = address;
    } else {
        PC += 3;
    }

    clock += ticks;

    debug_cpu("JP 0x%04X\n", address);
}

void CPU::jp_hl()
{
    PC = mmu->get16(reg16(HL));

    debug_cpu("JP (HL)\n");

    clock += 4;
}

void CPU::call()
{
    uint8_t ticks = 24;

    uint8_t opcode = mmu->get(PC);
    uint16_t address = mmu->get16(PC + 1);

    PC += 3;

    uint8_t high = (uint8_t)((PC & 0xFF00) >> 8);
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
        dec16(&reg[SP]);
        mmu->set(reg16(SP), high);
        dec16(&reg[SP]);
        mmu->set(reg16(SP), low);

        PC = address;
    }

    clock += ticks;

    debug_cpu("CALL %04X\n", address);
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

        debug_cpu("RET 0x%04X\n", mmu->get16(reg16(SP)));
    }

    /* RETI */
    if (opcode == 0xD9) {
        IME = true;
    }

    if (do_ret) {
        PC = mmu->get16(reg16(SP));
        inc16(&reg[SP]);
        inc16(&reg[SP]);
    }

    clock += ticks;

    debug_cpu("RET\n");
}

// Invert reg A
void CPU::cpl()
{
    reg[A] = ~reg[A];

    reg[F] = set_bit(reg[F], FN, 1);
    reg[F] = set_bit(reg[F], FH, 1);

    PC += 1;
    clock += 4;

    debug_cpu("CPL\n");
}

// Switch carry flag
void CPU::ccf()
{
    reg[F] = set_bit(reg[F], FN, 0);
    reg[F] = set_bit(reg[F], FH, 0);
    reg[F] = set_bit(reg[F], FC, get_bit(reg[F], FC) == 0);

    PC += 1;
    clock += 4;

    debug_cpu("CCF\n");
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

    debug_cpu("DAA\n");
}

// Set carry flag
void CPU::scf()
{
    reg[F] = set_bit(reg[F], FN, 0);
    reg[F] = set_bit(reg[F], FH, 0);
    reg[F] = set_bit(reg[F], FC, 1);

    PC += 1;
    clock += 4;

    debug_cpu("SCF\n");
}

void CPU::stop()
{
    // TODO

    PC += 2;
    clock += 4;

    debug_cpu("STOP\n");
}

void CPU::halt()
{
    // TODO

    PC += 1;
    clock += 4;

    debug_cpu("HALT\n");
}

void CPU::add()
{
    size_t ticks = 4;
    uint16_t value;

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
    case 0xE8:
        value = mmu->get(PC + 1);
        add16(&reg[SP], (uint8_t*) &value);

        reg[F] = set_bit(reg[F], FZ, 0);

        ticks = 16;
        PC += 1;
        break;
    default:
        add8();
        return;
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

    debug_cpu("INC\n");
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

    debug_cpu("DEC\n");
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

    debug_cpu("JR %d\n", increment);
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

        size_t dst_index = (opcode - 0x40) / 8;
        size_t src_index = (opcode - 0x40) % 8;

        uint8_t *dst = l_address[dst_index];
        uint8_t *src = l_address[src_index];

        size_t ticks = 4;
        if (src_index == 6 || dst_index == 6) {
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
        debug_cpu("LD BC,0x%02X%02X\n", mmu->get(PC + 2), mmu->get(PC + 3));
        ld16(&reg[BC], (uint8_t*)mmu->at(PC + 1), 3, 12);
        break;

    case 0x11:    // Loads 16-bit immediate to DE
        debug_cpu("LD DE,0x%02X%02X\n", mmu->get(PC + 2), mmu->get(PC + 3));
        ld16(&reg[DE], (uint8_t*)mmu->at(PC + 1), 3, 12);
        break;

    case 0x21:    // Loads 16-bit immediate to HL
        debug_cpu("LD HL,0x%02X%02X\n", mmu->get(PC + 2), mmu->get(PC + 3));
        ld16(&reg[HL], (uint8_t*)mmu->at(PC + 1), 3, 12);
        break;

    case 0x31:    // Loads 16-bit immediate to SP
        debug_cpu("LD SP,0x%02X%02X\n", mmu->get(PC + 2), mmu->get(PC + 3));
        ld16(&reg[SP], (uint8_t*)mmu->at(PC + 1), 3, 12);
        break;

    /* Load reg A into pointed address */
    case 0x02:    // Loads reg A to (BC)
        ld8_mmu(reg16(BC), &reg[A], 1, 8);
        break;

    case 0x12:    // Loads reg A to (DE)
        ld8_mmu(reg16(DE), &reg[A], 1, 8);
        break;

    case 0x22:    // Loads reg A to (HL), inc HL
        ld8_mmu(reg16(HL), &reg[A], 1, 8);
        inc16(&reg[HL]);
        break;

    case 0x32:    // Loads reg A to (HL), dec HL
        ld8_mmu(reg16(HL), &reg[A], 1, 8);
        dec16(&reg[HL]);
        break;

    /* Load 16-bit Sp to (immediate 16-bit) */
    case 0x08:
        debug_cpu("LD 0x%02X%02X,SP\n", mmu->get(PC + 2), mmu->get(PC + 3));
        address = mmu->get16(PC + 1);
        ld16((uint8_t*)mmu->at(address), &reg[SP], 3, 20);
        break;

    /* Load 8-bit immediate to reg */
    case 0x06:    // Loads 8-bit immediate to B
        ld8(&reg[B], (uint8_t*) mmu->at(PC + 1), 2, 8);
        break;

    case 0x16:    // Loads 8-bit immediate to D
        ld8(&reg[D], (uint8_t*) mmu->at(PC + 1), 2, 8);
        break;

    case 0x26:    // Loads 8-bit immediate to H
        ld8(&reg[H], (uint8_t*) mmu->at(PC + 1), 2, 8);
        break;

    case 0x36:    // Loads 8-bit immediate to (HL)
        ld8_mmu(reg16(HL), (uint8_t*) mmu->at(PC + 1), 2, 12);
        break;

    /* Load pointed address into A */
    case 0x0A:    // Loads (BC) to reg A
        ld8(&reg[A], (uint8_t*) mmu->at(reg16(BC)), 1, 8);
        break;

    case 0x1A:    // Loads (DE) to reg A
        ld8(&reg[A], (uint8_t*) mmu->at(reg16(DE)), 1, 8);
        break;

    case 0x2A:    // Loads (HL) to reg A, inc HL
        ld8(&reg[A], (uint8_t*) mmu->at(reg16(HL)), 1, 8);
        inc16(&reg[HL]);
        break;

    case 0x3A:    // Loads (HL) to reg A, dec HL
        ld8(&reg[A], (uint8_t*) mmu->at(reg16(HL)), 1, 8);
        dec16(&reg[HL]);
        break;

    /* Load 8-bit immediate to reg */
    case 0x0E:    // Loads 8-bit immediate to C
        ld8(&reg[C], (uint8_t*) mmu->at(PC + 1), 2, 8);
        break;

    case 0x1E:    // Loads 8-bit immediate to E
        ld8(&reg[E], (uint8_t*) mmu->at(PC + 1), 2, 8);
        break;

    case 0x2E:    // Loads 8-bit immediate to L
        ld8(&reg[L], (uint8_t*) mmu->at(PC + 1), 2, 8);
        break;

    case 0x3E:    // Loads 8-bit immediate to A
        ld8(&reg[A], (uint8_t*) mmu->at(PC + 1), 2, 8);
        break;

    /* Loads from/to 8-bit address */
    case 0xE0:      // Loads reg A to address immediate
        address = 0xFF00 + mmu->get(PC + 1);
        ld8_mmu(address, &reg[A], 2, 12);
        break;

    case 0xF0:      // Loads from address immediate to reg A
        address = 0xFF00 + mmu->get(PC + 1);
        ld8(&reg[A], (uint8_t*) mmu->at(address), 2, 12);
        break;

    case 0xE2:      // Loads from addres reg C to reg A
        ld8_mmu(0xFF00 + reg[C], &reg[A], 1, 8);
        break;

    case 0xF2:      // Loads from reg A to addres reg C
        ld8(&reg[A], (uint8_t*) mmu->at(0xFF00 + reg[C]), 1, 8);
        break;

    /* Loads from/to 16-bit address */
    case 0xEA:      // Loads reg A to address immediate
        address = mmu->get16(PC + 1);
        ld8_mmu(address, &reg[A], 3, 16);
        break;

    case 0xFA:      // Loads from address immediate to reg A
        address = mmu->get16(PC + 1);
        ld8(&reg[A], (uint8_t*) mmu->at(address), 3, 16);
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
    uint8_t opcode = mmu->get(PC);
    bool is_left = true;
    if (opcode == 0x0F || opcode == 0x1F) {
        is_left = false;
    }

    bool carry, old_carry = get_bit(reg[F], FC);
    reg[A] = rotate(reg[A], is_left, &carry);

    bool new_value = carry;

    /* Through carry flag */
    if (opcode == 0x17 || opcode == 0x1F) {
        new_value = old_carry;
    }

    if (is_left) {
        reg[A] = set_bit(reg[A], 0, new_value);
    } else {
        reg[A] = set_bit(reg[A], 7, new_value);
    }

    reg[F] = set_bit(reg[F], FZ, 0);
    reg[F] = set_bit(reg[F], FN, 0);
    reg[F] = set_bit(reg[F], FH, 0);
    reg[F] = set_bit(reg[F], FC, carry);

    PC += 1;
    clock += 4;

    debug_cpu("RLCA\n");
}

void CPU::nop()
{
    PC += 1;
    clock += 4;

    debug_cpu("NOP\n");
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

        /* Opcode to 0x10 set new byte to the shifted one */
        bool new_value = carry;

        /* Through carry flag */
        if (opcode >= 0x10) {
            new_value = old_carry;
        }

        if (left) {
            *address = set_bit(*address, 0, new_value);
        } else {
            *address = set_bit(*address, 7, new_value);
        }

        reg[F] = set_bit(reg[F], FZ, *address == 0);
        reg[F] = set_bit(reg[F], FN, 0);
        reg[F] = set_bit(reg[F], FH, 0);
        reg[F] = set_bit(reg[F], FC, carry);
    }
    /* Shitf Left (SLA) */
    else if (opcode < 0x28) {
        *address = shift(*address, true, &carry);

        reg[F] = set_bit(reg[F], FZ, *address == 0);
        reg[F] = set_bit(reg[F], FN, 0);
        reg[F] = set_bit(reg[F], FH, 0);
        reg[F] = set_bit(reg[F], FC, carry);
    }
    /* Shitf Right, keep b7 (SRA) */
    else if (opcode < 0x30) {
        bool b7 = get_bit(*address, 7);

        *address = shift(*address, false, &carry);

        *address = set_bit(*address, 7, b7);

        reg[F] = set_bit(reg[F], FZ, *address == 0);
        reg[F] = set_bit(reg[F], FN, 0);
        reg[F] = set_bit(reg[F], FH, 0);
        reg[F] = set_bit(reg[F], FC, 0);
    }
    /* Swap */
    else if (opcode < 0x38) {
        *address = swap(*address);

        reg[F] = set_bit(reg[F], FZ, *address == 0);
        reg[F] = set_bit(reg[F], FN, 0);
        reg[F] = set_bit(reg[F], FH, 0);
        reg[F] = set_bit(reg[F], FC, 0);
    }
    /* Shitf Right Logical (SRL) */
    else if (opcode < 0x40) {
        *address = shift(*address, false, &carry);

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

    debug_cpu("Prefix CB\n");
}

void CPU::or_xor_and_cp()
{
    const uint8_t *l_address[] = {
        &reg[B],
        &reg[C],
        &reg[D],
        &reg[E],
        &reg[H],
        &reg[L],
        (const uint8_t*) mmu->at(reg16(HL)),
        &reg[A]
    };

    uint8_t opcode = mmu->get(PC);
    size_t target_index = opcode % 8;
    const uint8_t *target = l_address[target_index];

    // OR/XOR/AND/CP with d8
    if (opcode > 0xC0) {
        target = (const uint8_t*) mmu->at(PC + 1);
        //debug_cpu("Target value: 0x%02X\n", *target);
        PC += 2;
        clock += 8;
    }
    // (HL) case
    else if (target_index == 6) {
        PC += 1;
        clock += 8;
    }
    else {
        PC += 1;
        clock += 4;
    }

    // CP
    if ((opcode >= 0xB8 && opcode < 0xC0) || opcode == 0xFE) {
        uint16_t result = reg[A] - *target;

        reg[F] = set_bit(reg[F], FZ, (result & 0x00FF) == 0);
        reg[F] = set_bit(reg[F], FN, 1);
        reg[F] = set_bit(reg[F], FH, (reg[A] & 0x0F) < (*target & 0x0F));
        reg[F] = set_bit(reg[F], FC, !(result & 0x100));   // Underflow

        debug_cpu("CP\n");
    }
    // OR
    else if ((opcode >= 0xB0 && opcode < 0xB8) || opcode == 0xF6) {
        reg[A] |= *target;

        reg[F] = set_bit(reg[F], FZ, reg[A] == 0);
        reg[F] = set_bit(reg[F], FN, 0);
        reg[F] = set_bit(reg[F], FH, 0);
        reg[F] = set_bit(reg[F], FC, 0);

        debug_cpu("OR\n");
    }
    // XOR
    else if ((opcode >= 0xA8 && opcode < 0xB0) || opcode == 0xEE) {
        reg[A] ^= *target;

        reg[F] = set_bit(reg[F], FZ, reg[A] == 0);
        reg[F] = set_bit(reg[F], FN, 0);
        reg[F] = set_bit(reg[F], FH, 0);
        reg[F] = set_bit(reg[F], FC, 0);

        debug_cpu("XOR\n");
    }
    // AND
    else if ((opcode >= 0xA0 && opcode < 0xA8) || opcode == 0xE6) {
        reg[A] &= *target;

        reg[F] = set_bit(reg[F], FZ, reg[A] == 0);
        reg[F] = set_bit(reg[F], FN, 0);
        reg[F] = set_bit(reg[F], FH, 1);
        reg[F] = set_bit(reg[F], FC, 0);

        debug_cpu("AND\n");
    }
}

/**
 * @brief      Handles SUB and SBC
 */
void CPU::sub()
{
    const uint8_t *l_address[] = {
        &reg[B],
        &reg[C],
        &reg[D],
        &reg[E],
        &reg[H],
        &reg[L],
        (const uint8_t*) mmu->at(reg16(HL)),
        &reg[A]
    };

    uint8_t opcode = mmu->get(PC);
    size_t target_index = opcode % 8;
    const uint8_t *target = l_address[target_index];

    // SUB/SBC d8
    if (opcode > 0xD0) {
        target = (const uint8_t*) mmu->at(PC + 1);
        PC += 2;
        clock += 8;
    }
    // (HL) case
    else if (target_index == 6) {
        PC += 1;
        clock += 8;
    } else {
        PC += 1;
        clock += 4;
    }

    uint16_t value = *target;

    bool half_carry = (reg[A] & 0x0F) < (value & 0x0F);

    uint16_t result = (0x0100 + reg[A]) - value;

    // SBC
    if (opcode >= 0x98 && opcode != 0xD6) {
        value = get_bit(reg[F], FC);

        // FH set when substracting carry flag
        half_carry |= (result & 0x0F) < (value & 0x0F);

        result -= value;

        debug_cpu("SBC\n");
    } else {
        debug_cpu("SUB\n");
    }

    reg[A] = (uint8_t) result;

    reg[F] = set_bit(reg[F], FZ, reg[A] == 0);
    reg[F] = set_bit(reg[F], FN, 1);
    reg[F] = set_bit(reg[F], FH, half_carry);
    reg[F] = set_bit(reg[F], FC, !(result & 0x100));   // Underflow
}

void CPU::ei()
{
    IME = true;

    PC += 1;
    clock += 4;
}

void CPU::di()
{
    IME = false;

    PC += 1;
    clock += 4;
}

void CPU::rst()
{
    uint8_t opcode = mmu->get(PC);

    PC += 1;

    uint8_t high = (uint8_t)((PC & 0xFF00) >> 4);
    uint8_t low = (uint8_t)(PC & 0x00FF);

    mmu->set(SP - 1, high);
    mmu->set(SP - 2, low);

    switch(opcode) {
    case 0xC7:
        PC = 0x00;
        break;
    case 0xCF:
        PC = 0x08;
        break;
    case 0xD7:
        PC = 0x10;
        break;
    case 0xDF:
        PC = 0x18;
        break;
    case 0xE7:
        PC = 0x20;
        break;
    case 0xEF:
        PC = 0x28;
        break;
    case 0xF7:
        PC = 0x30;
        break;
    case 0xFF:
        PC = 0x38;
        break;
    }

    clock += 16;
}
