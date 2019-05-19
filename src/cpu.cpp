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
    l_callback[0xF3] = &CPU::ei_di;
    l_callback[0xF4] = NULL;
    l_callback[0xF5] = &CPU::push;
    l_callback[0xF6] = &CPU::or_xor_and_cp;
    l_callback[0xF7] = &CPU::rst;
    l_callback[0xF8] = &CPU::ld;
    l_callback[0xF9] = &CPU::ld;
    l_callback[0xFA] = &CPU::ld;
    l_callback[0xFB] = &CPU::ei_di;
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
    halted = false;
}

/**
 * @brief      Execute the opcode pointed by PC
 * @return     false in case of crash
 */
bool CPU::step()
{
    handle_interrupts();

    if (halted) {
        clock += 4;
        return true;
    }

    uint8_t opcode = mmu->get(PC);

    // Some instructions are not meant to do anything, crash the system
    if (l_callback[opcode] == nullptr) {
        error("Not Implemented PC: 0x%04X\tOpcode: 0x%02X\n", PC, opcode);
        PC += 1; // Just in case

        // TODO: Crash?
        return false;
    }

    (*this.*l_callback[opcode])(opcode);

    return true;
}


/**
 * @brief      Set the Flag register value
 * @param[in]  flag   The flag to set/unset
 * @param[in]  value  The value of the flag
 */
void CPU::set_flag(size_t flag, bool value)
{
    reg[F] = set_bit(reg[F], flag, value) & 0xF0;
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
    uint8_t high = reg[i];
    uint8_t low = reg[i + 1];

    return (high << 8) + low;
}

/**
 * @brief      Gives a target operand based on the given index
 *
 *             Most opcodes operates on the same targets. First DEC in the
 *             opcodes will always be B, same for ADD, and so on. This function
 *             uses this order to simplify targets identification.
 * @param[in]  index  The index
 * @return     The target address.
 */
uint8_t *CPU::get_target(size_t index)
{
    uint8_t *l_address[] = {
        &reg[B],
        &reg[C],
        &reg[D],
        &reg[E],
        &reg[H],
        &reg[L],
        // WARNING: We ignore some memory cannot be written!
        (uint8_t*) mmu->at(reg16(HL)),
        &reg[A]
    };

    return l_address[index];
}


/**
 * @brief      Same as get_target but for 16bit registers
 * @param[in]  index  The index
 * @return     The target 16.
 */
uint8_t *CPU::get_target16(size_t index)
{
    uint8_t *l_address[] = {
        &reg[BC],
        &reg[DE],
        &reg[HL],
        &reg[SP]
    };

    return l_address[index];
}

// TODO: Simplify
void CPU::ld8_mmu(uint16_t address, const uint8_t* src, size_t size, size_t ticks)
{
    mmu->set(address, *(uint8_t*) src);
    PC += size;
    clock += ticks;
}

void CPU::ld8(uint8_t *dst, const uint8_t* src, size_t size, size_t ticks)
{
    memcpy(dst, src, sizeof(uint8_t));
    PC += size;
    clock += ticks;
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

    set_flag(FZ, *address == 0);
    set_flag(FN, 0);
    set_flag(FH, (*address & 0x0F) == 0x00); // Went from 0xXF to 0xX0
}

void CPU::dec8(uint8_t *address)
{
    *address = *address - 1;

    set_flag(FZ, *address == 0);
    set_flag(FN, 1);
    set_flag(FH, (*address & 0x0F) == 0x0F); // Went from 0xX0 to 0xXF
}

/**
 * @brief      Handles ADD and ADC
 */
void CPU::add8(uint8_t opcode)
{
    size_t target_index = opcode % 8;
    const uint8_t *target = get_target(target_index);

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
    }

    uint16_t result = reg[A] + value;

    // FH set when adding carry flag
    bool half_carry = (*target & 0xF0) < (value & 0xFFF0);

    half_carry |= (reg[A] & 0x0F) + (value & 0x0F) > 0x0F;

    reg[A] = (uint8_t) result;

    set_flag(FZ, reg[A] == 0);
    set_flag(FN, 0);
    set_flag(FH, half_carry);
    set_flag(FC, result > 0x00FF);  // Overflow
}

void CPU::add16(uint8_t *dst, const uint8_t *src)
{
    uint8_t *dh = dst;
    const uint8_t *sh = src;
    uint8_t *dl = dst + 1;
    const uint8_t *sl = src + 1;

    uint16_t dst16 = (*dh << 8) + *dl;
    uint16_t src16 = (*sh << 8) + *sl;

    uint16_t result = dst16 + src16;

    *dh = result >> 8;
    *dl = result;

    // Is the sum of upper byte only the same as result?
    bool half_carry = (((dst16 & 0xF000) + (src16 & 0xF000)) & 0xF000) != (result & 0xF000);

    //set_flag(FZ, *(uint16_t*) dst == 0x0000);
    set_flag(FN, 0);
    set_flag(FH, half_carry);
    set_flag(FC, result < dst16);
}

void CPU::addr8(uint8_t *dst, int value)
{
    uint8_t high = *dst;
    uint8_t low = *(dst + 1);

    uint16_t dst16 = (high << 8) + low;

    // The leading 1 is used to carry for underflow
    uint32_t result = 0x00010000 + dst16;

    result += value;

    set_flag(FZ, 0);
    set_flag(FN, 0);
    set_flag(FH, (result & 0x0F) < (dst16 & 0x0F));
    set_flag(FC, (result & 0xFF) < (dst16 & 0xFF));

    *dst = (result & 0x0000FF00) >> 8;
    *(dst + 1) = result;
}

void CPU::push(uint8_t opcode)
{
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
}

void CPU::pop(uint8_t opcode)
{
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

    // 4 lower bits of flag register must remain at 0
    if (opcode == 0xF1) {
        reg[F] &= 0xF0;
    }
}

void CPU::jp(uint8_t opcode)
{
    uint8_t ticks = 16;
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
}

void CPU::jp_hl(uint8_t /*opcode*/)
{
    PC = reg16(HL);
    clock += 4;
}

void CPU::call(uint8_t opcode)
{
    uint8_t ticks = 24;
    uint16_t address = mmu->get16(PC + 1);

    PC += 3;

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
        _call(address);
    }

    clock += ticks;
}

void CPU::ret(uint8_t opcode)
{
    PC += 1;

    /* RETI */
    if (opcode == 0xD9) {
        IME = true;
        clock += 16;
    }

    else if (opcode == 0xC9) {
        clock += 16;
    }
    /* Ret if not Z */
    else if ((opcode == 0xC0 && get_bit(reg[F], FZ)) ||
    /* Ret if not C */
        (opcode == 0xD0 && get_bit(reg[F], FC)) ||
    /* Ret if Z */
        (opcode == 0xC8 && get_bit(reg[F], FZ) == 0) ||
    /* Ret if C */
        (opcode == 0xD8 && get_bit(reg[F], FC) == 0)) {
        clock += 8;
        return;
    } else {
        clock += 20;
    }

    PC = mmu->get16(reg16(SP));
    inc16(&reg[SP]);
    inc16(&reg[SP]);
}

// Invert reg A
void CPU::cpl(uint8_t /*opcode*/)
{
    reg[A] = ~reg[A];

    set_flag(FN, 1);
    set_flag(FH, 1);

    PC += 1;
    clock += 4;
}

// Switch carry flag
void CPU::ccf(uint8_t /*opcode*/)
{
    set_flag(FN, 0);
    set_flag(FH, 0);
    set_flag(FC, get_bit(reg[F], FC) == 0);

    PC += 1;
    clock += 4;
}

void CPU::daa(uint8_t /*opcode*/)
{
    uint16_t value = reg[A];

    // Substraction
    if (get_flag(FN)) {
        // Lower digit
        if (get_flag(FH)) {
            value = (value - 0x06) & 0xFF;
        }

        // Higher digit
        if (get_flag(FC)) {
            value -= 0x60;
        }
    }

    // Addition
    else {
        // Lower digit
        if (get_flag(FH) || (value & 0xF) > 0x09) {
            value += 0x06;
        }

        // Higher digit
        if (get_flag(FC) || value > 0x9F) {
            value += 0x60;
        }
    }

    // DO NOT RESET C even if this is false!
    if (value & 0x100) {
        set_flag(FC, true);
    }

    value &= 0xFF;
    reg[A] = value;

    set_flag(FZ, value == 0);
    set_flag(FH, 0);

    PC += 1;
    clock += 4;
}

// Set carry flag
void CPU::scf(uint8_t /*opcode*/)
{
    set_flag(FN, 0);
    set_flag(FH, 0);
    set_flag(FC, 1);

    PC += 1;
    clock += 4;
}

void CPU::stop(uint8_t /*opcode*/)
{
    // TODO

    PC += 2;
    clock += 4;
}

void CPU::halt(uint8_t /*opcode*/)
{
    halted = true;

    PC += 1;
    clock += 4;
}

void CPU::add(uint8_t opcode)
{
    switch(opcode) {
    case 0x09:
    case 0x19:
    case 0x29:
    case 0x39:
        PC += 1;
        clock += 8;
        return add16(&reg[HL], get_target16(opcode >> 4));
    case 0xE8:
        addr8(&reg[SP], mmu->get_signed(PC + 1));
        PC += 2;
        clock += 16;
        return;
    default:
        return add8(opcode);
    }
}

void CPU::inc(uint8_t opcode)
{
    PC += 1;

    switch(opcode) {
    /* Inc 16-bit registers */
    case 0x03:
    case 0x13:
    case 0x23:
    case 0x33:
        clock += 8;
        return inc16(get_target16(opcode >> 4));
    /* Inc 8-bit registers */
    case 0x04:
    case 0x0C:
    case 0x14:
    case 0x1C:
    case 0x24:
    case 0x2C:
    case 0x34:
    case 0x3C:
        clock += 4;
        return inc8(get_target((opcode - 0x04) / 0x08));
    }
}

void CPU::dec(uint8_t opcode)
{
    PC += 1;

    switch(opcode) {
    /* Dec 16-bit registers */
    case 0x0B:
    case 0x1B:
    case 0x2B:
    case 0x3B:
        clock += 8;
        return dec16(get_target16(opcode >> 4));
    /* Dec 8-bit registers */
    case 0x05:
    case 0x0D:
    case 0x15:
    case 0x1D:
    case 0x25:
    case 0x2D:
    case 0x35:
    case 0x3D:
        clock += 4;
        return dec8(get_target((opcode - 0x05) / 0x08));
    }
}

void CPU::jr(uint8_t opcode)
{
    // Jump if flag Z is not set
    if ((opcode == 0x20 && get_bit(reg[F], FZ) == 0) ||
    // Jump if flag C is not set
        (opcode == 0x30 && get_bit(reg[F], FC) == 0) ||
    // Jump if flag Z is set
        (opcode == 0x28 && get_bit(reg[F], FZ)) ||
    // Jump if flag C is not set
        (opcode == 0x38 && get_bit(reg[F], FC)) ||
        (opcode == 0x18)) {
        PC += mmu->get_signed(PC + 1);
        clock += 4;
    }

    PC += 2;
    clock += 8;
}

void CPU::ld(uint8_t opcode)
{
    /* Generic cases */
    if (opcode >= 0x40 && opcode < 0x80) {
        size_t dst_index = (opcode - 0x40) / 8;
        size_t src_index = (opcode - 0x40) % 8;

        uint8_t *dst = get_target(dst_index);
        uint8_t *src = get_target(src_index);

        size_t ticks = 4;
        if (src_index == 6 || dst_index == 6) {
            ticks += 4;
        }

        return ld8(dst, src, 1, ticks);
    }

    uint16_t address;

    /* Less generic cases */
    switch (opcode) {
    /* Load 16-bit immediate to r16 */
    case 0x01:    // Loads 16-bit immediate to BC
    case 0x11:    // Loads 16-bit immediate to DE
    case 0x21:    // Loads 16-bit immediate to HL
    case 0x31:    // Loads 16-bit immediate to SP
        ld16(get_target16(opcode >> 4), (uint8_t*)mmu->at(PC + 1), 3, 12);
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
        addr8((uint8_t*) &reg[HL], mmu->get_signed(PC + 1));

        PC += 2;
        clock += 12;
        break;

    case 0xF9:      // Loads HL content to SP
        memcpy(&reg[SP], &reg[HL], sizeof(uint16_t));

        PC += 1;
        clock += 8;
        break;
    }
}

void CPU::rxa(uint8_t opcode)
{
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

    set_flag(FZ, 0);
    set_flag(FN, 0);
    set_flag(FH, 0);
    set_flag(FC, carry);

    PC += 1;
    clock += 4;
}

void CPU::nop(uint8_t /*opcode*/)
{
    PC += 1;
    clock += 4;
}

void CPU::prefix_CB(uint8_t opcode)
{
    opcode = mmu->get(PC + 1);
    uint8_t *address = get_target(opcode % 8);

    size_t offset = (opcode / 8) % 8;
    bool is_left = (opcode % 16) < 8;
    bool carry, old_carry = get_bit(reg[F], FC);

    /* Rotate */
    if (opcode < 0x20) {
        *address = rotate(*address, is_left, &carry);

        /* Opcode to 0x10 set new byte to the shifted one */
        bool new_value = carry;

        /* Through carry flag */
        if (opcode >= 0x10) {
            new_value = old_carry;
        }

        if (is_left) {
            *address = set_bit(*address, 0, new_value);
        } else {
            *address = set_bit(*address, 7, new_value);
        }

        set_flag(FZ, *address == 0);
        set_flag(FN, 0);
        set_flag(FH, 0);
        set_flag(FC, carry);
    }
    /* Shitf Left (SLA) */
    else if (opcode < 0x28) {
        *address = shift(*address, UTIL_LEFT, &carry);

        set_flag(FZ, *address == 0);
        set_flag(FN, 0);
        set_flag(FH, 0);
        set_flag(FC, carry);
    }
    /* Shitf Right, keep b7 (SRA) */
    else if (opcode < 0x30) {
        bool b0 = get_bit(*address, 0);
        bool b7 = get_bit(*address, 7);

        *address = shift(*address, UTIL_RIGHT, &carry);

        *address = set_bit(*address, 7, b7);

        set_flag(FZ, *address == 0);
        set_flag(FN, 0);
        set_flag(FH, 0);
        set_flag(FC, b0);
    }
    /* Swap */
    else if (opcode < 0x38) {
        *address = swap(*address);

        set_flag(FZ, *address == 0);
        set_flag(FN, 0);
        set_flag(FH, 0);
        set_flag(FC, 0);
    }
    /* Shitf Right Logical (SRL) */
    else if (opcode < 0x40) {
        *address = shift(*address, UTIL_RIGHT, &carry);

        set_flag(FZ, *address == 0);
        set_flag(FN, 0);
        set_flag(FH, 0);
        set_flag(FC, carry);
    }
    /* Get Bit */
    else if (opcode < 0x80) {
        set_flag(FZ, get_bit(*address, offset) == 0);
        set_flag(FN, 0);
        set_flag(FH, 1);
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

void CPU::or_xor_and_cp(uint8_t opcode)
{
    size_t target_index = opcode % 8;
    const uint8_t *target = get_target(target_index);

    // OR/XOR/AND/CP with d8
    if (opcode > 0xC0) {
        target = (const uint8_t*) mmu->at(PC + 1);
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
        // We add an extra 0x0100 so we can detect underflow if high byte is 0x00
        // after the comparison
        uint16_t result = 0x0100 + reg[A];
        result -= *target;

        set_flag(FZ, (result & 0x00FF) == 0);
        set_flag(FN, 1);
        set_flag(FH, (reg[A] & 0x0F) < (*target & 0x0F));
        set_flag(FC, !(result & 0x100));   // Underflow
    }
    // OR
    else if ((opcode >= 0xB0 && opcode < 0xB8) || opcode == 0xF6) {
        reg[A] |= *target;

        set_flag(FZ, reg[A] == 0);
        set_flag(FN, 0);
        set_flag(FH, 0);
        set_flag(FC, 0);
    }
    // XOR
    else if ((opcode >= 0xA8 && opcode < 0xB0) || opcode == 0xEE) {
        reg[A] ^= *target;

        set_flag(FZ, reg[A] == 0);
        set_flag(FN, 0);
        set_flag(FH, 0);
        set_flag(FC, 0);
    }
    // AND
    else if ((opcode >= 0xA0 && opcode < 0xA8) || opcode == 0xE6) {
        reg[A] &= *target;

        set_flag(FZ, reg[A] == 0);
        set_flag(FN, 0);
        set_flag(FH, 1);
        set_flag(FC, 0);
    }
}

/**
 * @brief      Handles SUB and SBC
 */
void CPU::sub(uint8_t opcode)
{
    size_t target_index = opcode % 8;
    const uint8_t *target = get_target(target_index);

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
    }

    reg[A] = (uint8_t) result;

    set_flag(FZ, reg[A] == 0);
    set_flag(FN, 1);
    set_flag(FH, half_carry);
    set_flag(FC, !(result & 0x100));   // Underflow
}

void CPU::ei_di(uint8_t opcode)
{
    IME = opcode == 0xFB;

    PC += 1;
    clock += 4;
}

void CPU::rst(uint8_t opcode)
{
    PC += 1;

    uint8_t high = PC >> 8;
    uint8_t low = PC;

    uint16_t sp = reg16(SP);

    mmu->set(sp - 1, high);
    mmu->set(sp - 2, low);

    sp -= 2;

    reg[SP] = sp >> 8;
    reg[SP + 1] = sp;

    PC = (opcode - 0xC7);
    clock += 16;
}


/**
 * @brief      Call that does not affect clock
 * @param[in]  address  The address
 */
void CPU::_call(uint16_t address)
{
    dec16(&reg[SP]);
    mmu->set(reg16(SP), PC >> 8);
    dec16(&reg[SP]);
    mmu->set(reg16(SP), PC);

    PC = address;
}


void CPU::handle_interrupts()
{
    // Disabled interruption
    if (!IME) {
        return;
    }

    uint8_t _ie = mmu->get(IE_ADDRESS);
    uint8_t _if = mmu->get(IF_ADDRESS);

    const size_t interrupt_count = 5;
    uint8_t interrupt_masks[] = {
        V_BLANK_MASK,
        LCD_STAT_MASK,
        TIMER_MASK,
        SERIAL_MASK,
        JOYPAD_MASK
    };

    bool interrupted = false;
    for (size_t i=0; i<interrupt_count; i++) {
        uint8_t mask = interrupt_masks[i];

        if (_ie & mask && _if & mask) {
            _call(0x0040 + (i * 0x0008));
            interrupted = true;
        }

        // Discard the interrupt
        mmu->set(IF_ADDRESS, _if ^ mask);

        // Serve only one interrupt at a time
        if (interrupted) {
            break;
        }
    }
}
