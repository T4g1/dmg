#include "test.h"

#include <initializer_list>

#include "log.h"
#include "dmg.h"

Cartridge *cart;
MMU *mmu;
PPU *ppu;
CPU *cpu;
Timer *timer;
Input *input;

/**
 * @brief      Init CPU to PC 0 and same value for all register except F
 * @param[in]  value  The value for all registers
 */
void init(uint8_t value)
{
    cpu->reset();

    cpu->PC = 0x00;

    cpu->reg[F] = 0x00;

    cpu->reg[B] = value;
    cpu->reg[C] = value;
    cpu->reg[D] = value;
    cpu->reg[E] = value;
    cpu->reg[H] = value;
    cpu->reg[L] = value;
    cpu->reg[A] = value;

    mmu->set_cartridge(cart);
    mmu->set(BOOT_ROM_ENABLE, 0x00);
}

/**
 * @brief      Store the given program at current PC position and execute the first
 *             command from it.
 * @param      program  The program to use
 */
void execute(std::initializer_list<uint8_t> program)
{
    mmu->load((uint8_t*)program.begin(), program.size(), cpu->PC);

    cpu->step();
}

bool test_MMU_ram()
{
    // Write to RAM
    mmu->set(0x8000, 0x00);
    mmu->set(0x8001, 0xDD);
    mmu->set(0x8002, 0xFF);
    mmu->set(0x8003, 0x12);

    ASSERT(mmu->get(0x8000) == 0x00);
    ASSERT(mmu->get(0x8001) == 0xDD);
    ASSERT(mmu->get(0x8002) == 0xFF);
    ASSERT(mmu->get(0x8003) == 0x12);

    // Write to ROM
    ASSERT(!mmu->set(0x0000, 0x00));
    ASSERT(!mmu->set(0x0001, 0xDD));
    ASSERT(!mmu->set(0x0002, 0xFF));
    ASSERT(!mmu->set(0x0003, 0x12));

    size_t size = 3;
    uint8_t program[] = { 0x01, 0x00, 0x11 };
    mmu->load(program, size);
    for (size_t i=0; i<size; i++) {
        ASSERT(mmu->get(i) == program[i]);
    }

    return true;
}


/****************************************************************
 *
 *      TEST CPU OPERATIONS
 *
 ****************************************************************/

bool test_CPU_NOP()
{
    init(0x00);

    // NOP
    cpu->PC = 0;
    uint8_t program[] = { 0x00 };
    mmu->load(program, 1);

    ASSERT(cpu->PC == 0);
    cpu->step();
    ASSERTV(cpu->PC == 1, "PC: %d\n", cpu->PC);

    return true;
}

bool test_CPU_LD_8bit()
{
    init(0x88);

    cpu->reg[A] = 0xDD;

    uint8_t program[] = {
        0x02,               // LD (BC), A
        0x12,               // LD (DE), A
        0x22,               // LD (HL+), A
        0x32,               // LD (HL-), A

        0x06, 0xAA,         // LD B, d8
        0x16, 0xAA,         // LD D, d8
        0x26, 0xAA,         // LD H, d8
        0x36, 0xAA,         // LD (HL), d8

        0x0A,               // LD A, (BC)
        0x1A,               // LD A, (DE)
        0x2A,               // LD A, (HL+)
        0x3A,               // LD A, (HL-)

        0x0E, 0xAA,         // LD C, d8
        0x1E, 0xAA,         // LD E, d8
        0x2E, 0xAA,         // LD L, d8
        0x3E, 0xAA,         // LD A, d8

    };
    mmu->load(program, 24);

    ASSERT(cpu->PC == 0);
    cpu->step();
    ASSERT(cpu->PC == 1);
    cpu->step();
    ASSERT(cpu->PC == 2);
    cpu->step();
    ASSERT(cpu->PC == 3);
    cpu->step();

    ASSERT(mmu->get(cpu->reg16(BC)) == 0xDD);
    ASSERT(mmu->get(cpu->reg16(DE)) == 0xDD);
    ASSERT(mmu->get(cpu->reg16(HL)) == 0xDD);
    ASSERT(mmu->get(cpu->reg16(HL) + 1) == 0xDD);
    ASSERTV(cpu->reg16(HL) == 0x8888, "HL: 0x%04X\n", cpu->reg16(HL));

    ASSERT(cpu->PC == 4);
    cpu->step();
    ASSERT(cpu->PC == 6);
    cpu->step();
    ASSERT(cpu->PC == 8);
    cpu->step();
    ASSERT(cpu->PC == 10);
    cpu->step();

    ASSERT(cpu->reg[B] == 0xAA);
    ASSERT(cpu->reg[D] == 0xAA);
    ASSERT(cpu->reg[H] == 0xAA);
    ASSERTV(cpu->reg16(HL) == 0xAA88, "HL: 0x%04X\n", cpu->reg16(HL));
    ASSERT(mmu->get(cpu->reg16(HL)) == 0xAA);

    ASSERTV(cpu->reg16(BC) == 0xAA88, "BC: 0x%04X\n", cpu->reg16(BC));
    ASSERTV(cpu->reg16(DE) == 0xAA88, "DE: 0x%04X\n", cpu->reg16(DE));

    mmu->set(cpu->reg16(BC), 0xCC);
    mmu->set(cpu->reg16(DE), 0xCC);
    mmu->set(cpu->reg16(HL), 0xCC);
    mmu->set(cpu->reg16(HL) + 1, 0xCC);

    ASSERT(cpu->PC == 12);
    cpu->step();
    ASSERT(cpu->reg[A] == 0xCC);

    ASSERT(cpu->PC == 13);
    cpu->step();
    ASSERT(cpu->reg[A] == 0xCC);

    ASSERT(cpu->PC == 14);
    cpu->step();
    ASSERT(cpu->reg[A] == 0xCC);

    ASSERT(cpu->PC == 15);
    cpu->step();
    ASSERTV(cpu->reg[A] == 0xCC, "HL+1: 0x%02X\n", mmu->get(cpu->reg16(HL) + 1));

    ASSERT(cpu->PC == 16);
    cpu->step();
    ASSERT(cpu->PC == 18);
    cpu->step();
    ASSERT(cpu->PC == 20);
    cpu->step();
    ASSERT(cpu->PC == 22);
    cpu->step();

    ASSERT(cpu->PC == 24);

    ASSERT(cpu->reg[C] == 0xAA);
    ASSERT(cpu->reg[E] == 0xAA);
    ASSERT(cpu->reg[L] == 0xAA);
    ASSERT(cpu->reg[A] == 0xAA);

    // Generic load 8-bit
    init(0x00);
    cpu->reg[A] = 0xEE;

    execute({ 0x4F });      // LD C, A

    ASSERT(cpu->reg[A] == 0xEE);
    ASSERT(cpu->reg[C] == 0xEE);

    execute({ 0x5F });      // LD E, A

    ASSERT(cpu->reg[A] == 0xEE);
    ASSERT(cpu->reg[E] == 0xEE);


    cpu->reg[H] = 0xFF;
    cpu->reg[L] = 0x11;

    execute({ 0x77 });      // LD (HL), A

    ASSERTV(
        mmu->get(cpu->reg16(HL)) == 0xEE,
        "A: 0x%02X HL: 0x%04X (HL): 0x%02X\n",
        cpu->reg[A],
        cpu->reg16(HL),
        mmu->get(cpu->reg16(HL))
    );

    mmu->set(0xFF10, 0x00);
    cpu->reg[A] = 0x04;

    execute({ 0xE0, 0x10 });      // LD (0xFF00 + 0x0F), A

    ASSERTV(mmu->get(0xFF10) & 0x04, "0xFF10: 0x%02X\n", mmu->get(0xFF10));

    return true;
}

bool test_CPU_LD_16bit()
{
    init(0x00);

    uint8_t program[] = {
        0x01, 0x00, 0x11,   // LD BC, d16
        0x11, 0x22, 0x33,   // LD DE, d16
        0x21, 0x44, 0xC5,   // LD HL, d16
        0x31, 0x66, 0x77,   // LD SP, d16
        0x08, 0x66, 0xC7,   // LD (a16), SP
        0xF9,               // LD SP, HL
        0xF8, 0x14          // LD HL, SP+r8
    };
    mmu->load(program, 18);

    ASSERT(cpu->PC == 0);
    cpu->step();
    ASSERT(cpu->PC == 3);
    cpu->step();
    ASSERT(cpu->PC == 6);
    cpu->step();
    ASSERT(cpu->PC == 9);
    cpu->step();
    ASSERT(cpu->PC == 12);

    ASSERTV(cpu->reg16(SP) == 0x7766, "SP=0x%04X", cpu->reg16(SP));

    cpu->step();
    ASSERT(cpu->PC == 15);

    ASSERT(cpu->reg16(BC) == 0x1100);
    ASSERT(cpu->reg16(DE) == 0x3322);
    ASSERT(cpu->reg16(HL) == 0xC544);
    ASSERTV(cpu->reg16(SP) == 0x7766, "SP=0x%04X", cpu->reg16(SP));
    ASSERTV(mmu->get16(0xC766) == 0x7766, "0xC766: 0x%04X\n", mmu->get16(0xC766));

    cpu->step();
    ASSERTV(cpu->PC == 16, "PC=0x%04X", cpu->PC);
    cpu->step();
    ASSERTV(cpu->PC == 18, "PC=0x%04X", cpu->PC);

    ASSERTV(cpu->reg16(SP) == 0xC544, "SP=0x%04X", cpu->reg16(SP));
    ASSERTV(cpu->reg16(HL) == 0xC544 + 0x14, "HL=0x%04X", cpu->reg16(HL));

    return true;
}

bool test_CPU_INC()
{
    init(0x0F);

    uint8_t program[] = {
        0x03,       // INC BC
        0x13,       // INC DE
        0x23,       // INC HL
        0x33,       // INC SP
        0x04,       // INC B
        0x14,       // INC D
        0x24,       // INC H
        0x34,       // INC (HL)
        0x0C,       // INC C
        0x1C,       // INC E
        0x2C,       // INC L
        0x3C,       // INC A
    };
    mmu->load(program, 12);

    cpu->step();
    ASSERT(cpu->reg16(BC) == 0x0F10);
    cpu->step();
    ASSERT(cpu->reg16(DE) == 0x0F10);
    cpu->step();
    ASSERT(cpu->reg16(HL) == 0x0F10);

    uint16_t old_SP = cpu->reg16(SP);
    cpu->step();
    ASSERTV(cpu->reg16(SP) == old_SP + 1, "SP: 0x%04X\n", cpu->reg16(SP));

    cpu->step();
    ASSERTV(cpu->reg[B] == 0x10, "B: 0x%02X\n", cpu->reg[B]);
    cpu->step();
    ASSERT(cpu->reg[D] == 0x10);
    cpu->step();
    ASSERT(cpu->reg[H] == 0x10);

    cpu->reg[H] = 0x80;
    cpu->reg[L] = 0x00;
    mmu->set(cpu->reg16(HL), 0x00);

    cpu->step();
    ASSERT(mmu->get(cpu->reg16(HL)) == 0x01);

    cpu->step();
    ASSERT(cpu->reg[C] == 0x11);
    cpu->step();
    ASSERT(cpu->reg[E] == 0x11);
    cpu->step();
    ASSERTV(cpu->reg[L] == 0x01, "L: 0x%02X\n", cpu->reg[L]);
    cpu->step();
    ASSERT(cpu->reg[A] == 0x10);

    return true;
}

bool test_CPU_ADD()
{
    init(0x44);

    cpu->reg[H] = 0x80;
    cpu->reg[L] = 0x00;

    mmu->set(0x8000, 0xFF);

    uint8_t program[] = {
        0x80,       // ADD A,B
        0x81,       // ADD A,C
        0x82,       // ADD A,D
        0x83,       // ADD A,E
        0x84,       // ADD A,H
        0x85,       // ADD A,L
        0x86,       // ADD A,(HL)
        0x87,       // ADD A,A

        0x88,       // ADC A,B

        0xE8, 0x11, // ADD SP,r8
    };
    mmu->load(program, 11);

    cpu->reg[A] = 0x11;
    cpu->step();
    ASSERT(cpu->reg[A] == 0x55);

    cpu->reg[A] = 0x11;
    cpu->step();
    ASSERT(cpu->reg[A] == 0x55);

    cpu->reg[A] = 0x11;
    cpu->step();
    ASSERT(cpu->reg[A] == 0x55);

    cpu->reg[A] = 0x11;
    cpu->step();
    ASSERT(cpu->reg[A] == 0x55);

    cpu->reg[A] = 0x11;
    cpu->step();
    ASSERT(cpu->reg[A] == 0x91);

    cpu->reg[A] = 0x11;
    cpu->step();
    ASSERT(cpu->reg[A] == 0x11);

    cpu->reg[A] = 0x11;
    cpu->step();
    ASSERT(cpu->get_flag(FH));
    ASSERT(cpu->get_flag(FC));
    ASSERT(cpu->reg[A] == 0x10);

    cpu->reg[A] = 0x11;
    cpu->step();
    ASSERT(cpu->reg[A] == 0x22);

    cpu->reg[A] = 0x00;
    cpu->reg[B] = 0xFF;
    cpu->reg[F] = 0xFF;
    cpu->step();
    ASSERT(cpu->get_flag(FH));
    ASSERT(cpu->get_flag(FC));
    ASSERT(cpu->reg[A] == 0x00);

    cpu->step();
    ASSERTV(cpu->reg16(SP) == 0x000F, "SP: 0x%04X\n", cpu->reg16(SP));

    return true;
}

bool test_CPU_SUB()
{
    init(0x11);

    cpu->reg[H] = 0x80;
    cpu->reg[L] = 0x00;

    mmu->set(0x8000, 0x11);

    uint8_t program[] = {
        0x90,       // SUB A,B
        0x91,       // SUB A,C
        0x92,       // SUB A,D
        0x93,       // SUB A,E
        0x94,       // SUB A,H
        0x95,       // SUB A,L
        0x96,       // SUB A,(HL)
        0x97,       // SUB A,A

        0x98,       // SBC A,B
    };
    mmu->load(program, 9);

    cpu->reg[A] = 0x44;
    cpu->step();
    ASSERT(cpu->reg[A] == 0x33);

    cpu->reg[A] = 0x44;
    cpu->step();
    ASSERT(cpu->reg[A] == 0x33);

    cpu->reg[A] = 0x44;
    cpu->step();
    ASSERT(cpu->reg[A] == 0x33);

    cpu->reg[A] = 0x44;
    cpu->step();
    ASSERT(cpu->reg[A] == 0x33);

    cpu->reg[A] = 0x44;
    cpu->step();
    ASSERT(cpu->reg[A] == 0xC4);
    ASSERT(cpu->get_flag(FC));

    cpu->reg[A] = 0x44;
    cpu->step();
    ASSERTV(cpu->reg[A] == 0x44, "A: %02X\n", cpu->reg[A]);

    cpu->reg[A] = 0x44;
    cpu->step();
    ASSERT(cpu->reg[A] == 0x33);

    cpu->reg[A] = 0x11;
    cpu->step();
    ASSERT(cpu->reg[A] == 0x00);

    cpu->reg[A] = 0x00;
    cpu->reg[B] = 0xFF;
    cpu->reg[F] = 0xFF;
    cpu->step();
    ASSERT(cpu->get_flag(FH));
    ASSERT(cpu->get_flag(FC));
    ASSERTV(cpu->reg[A] == 0x00, "A: %02X\n", cpu->reg[A]);

    return true;
}

bool test_CPU_XOR()
{
    init(0x00);

    cpu->reg[A] = 0x0F;
    cpu->reg[B] = 0x13;

    execute({ 0xA8 });  // XOR B
    ASSERTV(cpu->reg[A] == 0x1C, "A: %02X\n", cpu->reg[A]);

    cpu->reg[A] = 0x0F;
    cpu->reg[B] = 0x0F;

    execute({ 0xA8 });  // XOR B
    ASSERT(cpu->reg[A] == 0x00);
    ASSERT(cpu->get_flag(FZ));

    cpu->reg[A] = 0x0F;
    cpu->reg[B] = 0x0F;

    execute({ 0xEE, 0x0F });  // XOR d8
    ASSERTV(cpu->reg[A] == 0x00, "A: %02X\n", cpu->reg[A]);
    ASSERT(cpu->get_flag(FZ));

    return true;
}

bool test_CPU_OR()
{
    init(0x00);

    cpu->reg[A] = 0x0F;
    cpu->reg[B] = 0x13;

    execute({ 0xB0 });  // OR B
    ASSERTV(cpu->reg[A] == 0x1F, "A: %02X\n", cpu->reg[A]);

    cpu->reg[A] = 0x00;
    cpu->reg[B] = 0x00;

    execute({ 0xB0 });  // OR B
    ASSERT(cpu->reg[A] == 0x00);
    ASSERT(cpu->get_flag(FZ));

    return true;
}

bool test_CPU_AND()
{
    init(0x00);

    cpu->reg[A] = 0x0F;
    cpu->reg[B] = 0x13;

    execute({ 0xA0 });  // AND B
    ASSERTV(cpu->reg[A] == 0x03, "A: %02X\n", cpu->reg[A]);

    cpu->reg[A] = 0x0F;
    cpu->reg[B] = 0xF0;

    execute({ 0xA0 });  // AND B
    ASSERT(cpu->reg[A] == 0x00);
    ASSERT(cpu->get_flag(FZ));

    return true;
}

bool test_CPU_CP()
{
    init(0x00);

    cpu->reg[A] = 0x09;
    execute({ 0xFE, 0x90 });        // CP 0x90
    ASSERT(!cpu->get_flag(FZ));

    cpu->reg[A] = 0x90;
    execute({ 0xFE, 0x90 });        // CP 0x90
    ASSERT(cpu->get_flag(FZ));

    return true;
}

bool test_CPU_CALL_RET()
{
    init(0x00);

    size_t size = 13;
    uint8_t program[] = {
        0xCD, 0x0A, 0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x3E, 0x4C,
        0xC9
    };
    mmu->load(program, size);

    cpu->step();
    ASSERTV(cpu->PC == 10, "PC: 0x%02X\n", cpu->PC);

    cpu->step();
    ASSERTV(cpu->PC == 12, "PC: 0x%02X\n", cpu->PC);

    cpu->step();

    //cpu->display_registers();
    //mmu->dump(0xFFF0, 0xFFFF);

    ASSERT(cpu->reg[A] == 0x4C);
    ASSERTV(cpu->PC == 3, "PC: 0x%02X\n", cpu->PC);

    execute({ 0x31, 0xFF, 0xCF });
    ASSERT(cpu->reg16(SP) == 0xCFFF);

    init(0x00);

    mmu->load(program, size);

    //cpu->display_registers();
    //mmu->dump(0xCF00, 0xCFFF);

    cpu->step();
    ASSERTV(cpu->PC == 10, "PC: 0x%02X\n", cpu->PC);

    cpu->step();
    ASSERTV(cpu->PC == 12, "PC: 0x%02X\n", cpu->PC);

    cpu->step();

    //cpu->display_registers();
    //mmu->dump(0xFFF0, 0xFFFF);

    ASSERT(cpu->reg[A] == 0x4C);
    ASSERTV(cpu->PC == 3, "PC: 0x%02X\n", cpu->PC);

    return true;
}

bool test_CPU_RST()
{
    init(0x00);

    uint8_t program[] = {
        0x3C,
        0xC7
    };
    mmu->load(program, 2);

    cpu->step();
    cpu->step();
    cpu->step();
    cpu->step();
    cpu->step();
    cpu->step();
    cpu->step();
    cpu->step();
    ASSERTV(cpu->PC == 0, "PC: 0x%02X\n", cpu->PC);
    ASSERTV(cpu->reg[A] == 4, "A: 0x%02X\n", cpu->reg[A]);

    return true;
}

bool test_CPU_PUSH_POP()
{
    init(0x00);

    cpu->reg[A] = 0x11;
    cpu->reg[F] = 0x22;
    cpu->reg[B] = 0x33;
    cpu->reg[C] = 0x44;
    cpu->reg[D] = 0x55;
    cpu->reg[E] = 0x66;
    cpu->reg[H] = 0x77;
    cpu->reg[L] = 0x88;

    size_t size = 8;
    uint8_t program[] = {
        0xC5,       // PUSH BC
        0xD5,       // PUSH DE
        0xE5,       // PUSH HL
        0xF5,       // PUSH AF

        0xC1,       // POP BC
        0xD1,       // POP DE
        0xE1,       // POP HL
        0xF1,       // POP AF
    };
    mmu->load(program, size);

    cpu->step();
    cpu->step();
    cpu->step();
    cpu->step();

    ASSERT(cpu->reg[B] == 0x33);
    ASSERT(cpu->reg[C] == 0x44);

    ASSERT(cpu->reg[D] == 0x55);
    ASSERT(cpu->reg[E] == 0x66);

    ASSERT(cpu->reg[H] == 0x77);
    ASSERT(cpu->reg[L] == 0x88);

    ASSERT(cpu->reg[A] = 0x11);
    ASSERT(cpu->reg[F] = 0x22);

    cpu->step();
    cpu->step();
    cpu->step();
    cpu->step();

    ASSERTV(cpu->reg[B] == 0x11, "B: 0x%02X\n", cpu->reg[B]);
    ASSERTV(cpu->reg[C] == 0x22, "C: 0x%02X\n", cpu->reg[C]);

    ASSERT(cpu->reg[D] == 0x77);
    ASSERT(cpu->reg[E] == 0x88);

    ASSERT(cpu->reg[H] == 0x55);
    ASSERT(cpu->reg[L] == 0x66);

    ASSERT(cpu->reg[A] = 0x33);
    ASSERT(cpu->reg[F] = 0x44);

    return true;
}

bool test_CPU_RLA_RLCA()
{
    init(0xF0);

    execute({ 0x07 });      // RLCA
    ASSERTV(cpu->reg[A] == 0b11100001, "A: 0x%02X\n", cpu->reg[A]);
    ASSERT(cpu->get_flag(FC));          // Test carry flag

    init(0xF0);

    execute({ 0x17 });      // RLA
    ASSERTV(cpu->reg[A] == 0b11100000, "A: 0x%02X\n", cpu->reg[A]);
    ASSERT(cpu->get_flag(FC));          // Test carry flag

    init(0x0F);

    execute({ 0x0F });      // RRCA
    ASSERTV(cpu->reg[A] == 0b10000111, "A: 0x%02X\n", cpu->reg[A]);
    ASSERT(cpu->get_flag(FC));          // Test carry flag

    init(0x0F);

    execute({ 0x1F });      // RRA
    ASSERTV(cpu->reg[A] == 0b00000111, "A: 0x%02X\n", cpu->reg[A]);
    ASSERT(cpu->get_flag(FC));          // Test carry flag

    return true;
}

bool test_CPU_INC_DEC()
{
    init(0xF0);

    execute({ 0x04 });      // INC B
    ASSERT(cpu->reg[B] == 0xF1);

    init(0xFF);

    execute({ 0x04 });      // INC B
    ASSERT(cpu->reg[B] == 0x00);

    init(0xF0);

    execute({ 0x05 });      // DEC B
    ASSERT(cpu->reg[B] == 0xEF);

    init(0x00);

    execute({ 0x05 });      // DEC B
    ASSERT(cpu->reg[B] == 0xFF);

    init(0x01);

    execute({ 0x05 });      // DEC B
    ASSERT(cpu->reg[B] == 0x00);
    ASSERT(cpu->get_flag(FZ));

    execute({ 0x3D });      // DEC A
    ASSERT(cpu->reg[A] == 0x00);
    ASSERT(cpu->get_flag(FZ));
    ASSERT(!cpu->get_flag(FH));

    return true;
}

bool test_CPU_JP()
{
    init(0x00);

    size_t size = 1;
    uint8_t program[] = {
        0xE9,       // JP (HL)
    };
    mmu->load(program, size);

    cpu->reg[H] = 0x0B;
    cpu->reg[L] = 0x8D;

    cpu->step();

    ASSERTV(cpu->PC == 0x0B8D, "B: 0x%04X\n", cpu->PC);

    return true;
}

bool test_CPU_POP_AF()
{
    init(0x00);

    size_t size = 19;
    uint8_t program[] = {
        0x00,
        0x01, 0x00, 0x12,       // ld   bc,$1200        ; 0x01
        0xC5,                   // push bc              ; 0x04
        0xF1,                   // pop  af              ; 0x05
        0xF5,                   // push af              ; 0x06
        0xD1,                   // pop  de              ; 0x07
        0x79,                   // ld   a,c             ; 0x08
        0xE6, 0xF0,             // and  $F0             ; 0x09
        0xBB,                   // cp   e               ; 0x0B
        0xC2, 0x00, 0x00,       // jp   nz,test_failed  ; 0x0C
        0x04,                   // inc  b               ; 0x0F
        0x0C,                   // inc  c               ; 0x10
        0x20, 0xF1,             // jr   nz,-            ; 0x11
    };
    mmu->load(program, size);

    cpu->step();
    ASSERT(cpu->PC == 0x01);

    while (cpu->PC > 0x00 && cpu->PC < 0x13) {
        //cpu->display_registers();
        //mmu->dump(0xFF00, 0xFFFE);

        cpu->step();
    }

    //cpu->display_registers();
    //mmu->dump(0xFF00, 0xFFFE);

    ASSERTV(cpu->PC == 0x13, "PC: 0x%04X\n", cpu->PC);

    return true;
}

bool test_CPU_DAA()
{
    // Addition
    for (size_t a=0; a<99; a++) {
        for (size_t b=0; b<99; b++) {
            init(0x00);

            size_t result = a + b;

            uint8_t a_bcd = ((a / 10) << 4) + (a % 10);
            uint8_t b_bcd = ((b / 10) << 4) + (b % 10);
            uint8_t result_bcd = (((result / 10) % 10) << 4) + (result % 10);

            cpu->reg[A] = a_bcd;
            cpu->reg[B] = b_bcd;

            execute({ 0x80 });  // ADD A,B
            execute({ 0x27 });  // DAA

            ASSERT_QUIET_SUCCESS(
                cpu->reg[A] == result_bcd,
                "0x%02X + 0x%02X = 0x%02X expected 0x%02X\n",
                a_bcd, b_bcd, cpu->reg[A], result_bcd
            );
        }
    }

    // Addition OK
    ASSERT(true);

    // Substraction
    for (size_t a=0; a<99; a++) {
        for (size_t b=0; b<99; b++) {
            init(0x00);

            size_t result = ((100 + a) - b);

            uint8_t a_bcd = ((a / 10) << 4) + (a % 10);
            uint8_t b_bcd = ((b / 10) << 4) + (b % 10);
            uint8_t result_bcd = (((result / 10) % 10) << 4) + (result % 10);

            cpu->reg[A] = a_bcd;
            cpu->reg[B] = b_bcd;

            execute({ 0x90 });  // SUB B
            execute({ 0x27 });  // DAA

            ASSERT_QUIET_SUCCESS(
                cpu->reg[A] == result_bcd,
                "0x%02X - 0x%02X = 0x%02X expected 0x%02X\n",
                a_bcd, b_bcd, cpu->reg[A], result_bcd
            );
        }
    }

    // Substraction OK
    ASSERT(true);

    return true;
}

bool test_CPU_Op_SP()
{
    init(0x00);

    // ADD 2 to SP
    execute({ 0x31, 0x00, 0x00 });
    execute({ 0xE8, 0x02 });

    ASSERTV(cpu->reg16(SP) == 0x02, "SP: 0x%04X\n", cpu->reg16(SP));

    init(0x00);

    // ADD -1 to SP
    execute({ 0x31, 0x001, 0x00 });
    execute({ 0xE8, 0xFF });

    ASSERTV(cpu->reg16(SP) == 0x00, "SP: 0x%04X\n", cpu->reg16(SP));

    // ADD HL, SP
    execute({ 0x21, 0xEE, 0xCC });
    ASSERTV(cpu->reg16(HL) == 0xCCEE, "HL: 0x%04X\n", cpu->reg16(HL));
    execute({ 0x31, 0x11, 0x22 });
    ASSERTV(cpu->reg16(SP) == 0x2211, "SP: 0x%04X\n", cpu->reg16(SP));
    execute({ 0x39 });

    ASSERTV(cpu->reg16(HL) == 0xEEFF, "HL: 0x%04X\n", cpu->reg16(HL));

    return true;
}


/****************************************************************
 *
 *      TEST BITWISE OPERATIONS
 *
 ****************************************************************/

bool test_BIN_RLC()
{
    init(0xF0);

    execute({ 0xCB, 0x00 });    // RLC B
    ASSERT(cpu->reg[B] == 0b11100001);
    ASSERT(cpu->reg[F] == 0x10);    // Test carry flag
    execute({ 0xCB, 0x00 });    // RLC B
    ASSERT(cpu->reg[B] == 0b11000011);
    execute({ 0xCB, 0x01 });    // RLC C
    ASSERT(cpu->reg[C] == 0b11100001);
    execute({ 0xCB, 0x02 });    // RLC D
    ASSERT(cpu->reg[D] == 0b11100001);
    execute({ 0xCB, 0x03 });    // RLC E
    ASSERT(cpu->reg[E] == 0b11100001);
    execute({ 0xCB, 0x04 });    // RLC H
    ASSERT(cpu->reg[H] == 0b11100001);
    execute({ 0xCB, 0x05 });    // RLC L
    ASSERT(cpu->reg[L] == 0b11100001);
    execute({ 0xCB, 0x07 });    // RLC A
    ASSERT(cpu->reg[A] == 0b11100001);

    mmu->set(cpu->reg16(HL), 0xF0);
    execute({ 0xCB, 0x06 });    // RLC (HL)
    ASSERT(mmu->get(cpu->reg16(HL)) == 0b11100001);

    // Test zero flag
    init(0x00);
    execute({ 0xCB, 0x08 });
    ASSERT(cpu->reg[F] == 0b10000000);

    return true;
}

bool test_BIN_RRC()
{
    init(0x0F);

    execute({ 0xCB, 0x08 });    // RRC B
    ASSERT(cpu->reg[B] == 0b10000111);
    execute({ 0xCB, 0x08 });    // RRC B
    ASSERT(cpu->reg[B] == 0b11000011);
    execute({ 0xCB, 0x09 });    // RRC C
    ASSERT(cpu->reg[C] == 0b10000111);
    execute({ 0xCB, 0x0A });    // RRC D
    ASSERT(cpu->reg[D] == 0b10000111);
    execute({ 0xCB, 0x0B });    // RRC E
    ASSERT(cpu->reg[E] == 0b10000111);
    execute({ 0xCB, 0x0C });    // RRC H
    ASSERT(cpu->reg[H] == 0b10000111);
    execute({ 0xCB, 0x0D });    // RRC L
    ASSERT(cpu->reg[L] == 0b10000111);
    execute({ 0xCB, 0x0F });    // RRC A
    ASSERT(cpu->reg[A] == 0b10000111);
    ASSERT(cpu->reg[F] == 0x10);    // Test carry flag

    mmu->set(cpu->reg16(HL), 0x0F);
    execute({ 0xCB, 0x0E });    // RRC (HL)
    ASSERT(mmu->get(cpu->reg16(HL)) == 0b10000111);

    // Test zero flag
    init(0x00);
    execute({ 0xCB, 0x08 });
    ASSERT(cpu->reg[F] == 0b10000000);

    return true;
}

bool test_BIN_RL()
{
    init(0xF0);

    execute({ 0xCB, 0x10 });
    ASSERTV(cpu->reg[B] == 0b11100000, "B=0x%02X F=0x%02X\n", cpu->reg[B], cpu->reg[F]);
    execute({ 0xCB, 0x10 });
    ASSERT(cpu->reg[B] == 0b11000001);
    execute({ 0xCB, 0x11 });
    ASSERT(cpu->reg[C] == 0b11100001);
    execute({ 0xCB, 0x12 });
    ASSERT(cpu->reg[D] == 0b11100001);
    execute({ 0xCB, 0x13 });
    ASSERT(cpu->reg[E] == 0b11100001);
    execute({ 0xCB, 0x14 });
    ASSERT(cpu->reg[H] == 0b11100001);
    execute({ 0xCB, 0x15 });
    ASSERT(cpu->reg[L] == 0b11100001);
    execute({ 0xCB, 0x17 });
    ASSERT(cpu->reg[A] == 0b11100001);
    ASSERT(cpu->reg[F] == 0x10);    // Test carry flag

    mmu->set(cpu->reg16(HL), 0xF0);
    execute({ 0xCB, 0x16 });
    ASSERT(mmu->get(cpu->reg16(HL)) == 0b11100001);

    // Test zero flag
    init(0x00);
    execute({ 0xCB, 0x10 });
    ASSERT(cpu->reg[F] == 0b10000000);

    return true;
}

bool test_BIN_RR()
{
    init(0x0F);

    execute({ 0xCB, 0x18 });
    ASSERT(cpu->reg[B] == 0b00000111);
    execute({ 0xCB, 0x18 });
    ASSERT(cpu->reg[B] == 0b10000011);
    execute({ 0xCB, 0x19 });
    ASSERT(cpu->reg[C] == 0b10000111);
    execute({ 0xCB, 0x1A });
    ASSERT(cpu->reg[D] == 0b10000111);
    execute({ 0xCB, 0x1B });
    ASSERT(cpu->reg[E] == 0b10000111);
    execute({ 0xCB, 0x1C });
    ASSERT(cpu->reg[H] == 0b10000111);
    execute({ 0xCB, 0x1D });
    ASSERT(cpu->reg[L] == 0b10000111);
    execute({ 0xCB, 0x1F });
    ASSERT(cpu->reg[A] == 0b10000111);
    ASSERT(cpu->reg[F] == 0x10);    // Test carry flag

    mmu->set(cpu->reg16(HL), 0x0F);
    execute({ 0xCB, 0x1E });
    ASSERT(mmu->get(cpu->reg16(HL)) == 0b10000111);

    // Test zero flag
    init(0x00);
    execute({ 0xCB, 0x18 });
    ASSERT(cpu->reg[F] == 0b10000000);

    return true;
}

bool test_BIN_SLA()
{
    init(0xF0);

    execute({ 0xCB, 0x20 });
    ASSERTV(cpu->reg[B] == 0b11100000, "value=%02X\n", cpu->reg[B]);
    execute({ 0xCB, 0x20 });
    ASSERT(cpu->reg[B] == 0b11000000);
    execute({ 0xCB, 0x21 });
    ASSERT(cpu->reg[C] == 0b11100000);
    execute({ 0xCB, 0x22 });
    ASSERT(cpu->reg[D] == 0b11100000);
    execute({ 0xCB, 0x23 });
    ASSERT(cpu->reg[E] == 0b11100000);
    execute({ 0xCB, 0x24 });
    ASSERT(cpu->reg[H] == 0b11100000);
    execute({ 0xCB, 0x25 });
    ASSERT(cpu->reg[L] == 0b11100000);
    execute({ 0xCB, 0x27 });
    ASSERT(cpu->reg[A] == 0b11100000);
    ASSERT(cpu->reg[F] == 0x10);    // Test carry flag

    mmu->set(cpu->reg16(HL), 0xF0);
    execute({ 0xCB, 0x26 });
    ASSERT(mmu->get(cpu->reg16(HL)) == 0b11100000);

    init(0x80);
    execute({ 0xCB, 0x20 });
    ASSERT(cpu->reg[F] == 0x90);    // Test zero flag flag

    return true;
}

bool test_BIN_SRA()
{
    init(0b10001111);

    execute({ 0xCB, 0x28 });
    ASSERTV(cpu->reg[B] == 0b11000111, "value=%02X\n", cpu->reg[B]);
    execute({ 0xCB, 0x28 });
    ASSERT(cpu->reg[B] == 0b11100011);
    execute({ 0xCB, 0x29 });
    ASSERT(cpu->reg[C] == 0b11000111);
    execute({ 0xCB, 0x2A });
    ASSERT(cpu->reg[D] == 0b11000111);
    execute({ 0xCB, 0x2B });
    ASSERT(cpu->reg[E] == 0b11000111);
    execute({ 0xCB, 0x2C });
    ASSERT(cpu->reg[H] == 0b11000111);
    execute({ 0xCB, 0x2D });
    ASSERT(cpu->reg[L] == 0b11000111);
    execute({ 0xCB, 0x2F });
    ASSERT(cpu->reg[A] == 0b11000111);

    mmu->set(cpu->reg16(HL), 0b10001111);
    execute({ 0xCB, 0x2E });
    ASSERT(mmu->get(cpu->reg16(HL)) == 0b11000111);

    init(0x01);
    execute({ 0xCB, 0x28 });
    ASSERTV(cpu->reg[F] == 0x90, "F:%02X\n", cpu->reg[F]);    // Test zero/carry flag flag

    return true;
}

bool test_BIN_SWAP()
{
    init(0XF0);

    execute({ 0xCB, 0x30 });
    ASSERTV(cpu->reg[B] == 0X0F, "value=%02X\n", cpu->reg[B]);
    execute({ 0xCB, 0x30 });
    ASSERT(cpu->reg[B] == 0XF0);
    execute({ 0xCB, 0x31 });
    ASSERT(cpu->reg[C] == 0X0F);
    execute({ 0xCB, 0x32 });
    ASSERT(cpu->reg[D] == 0X0F);
    execute({ 0xCB, 0x33 });
    ASSERT(cpu->reg[E] == 0X0F);
    execute({ 0xCB, 0x34 });
    ASSERT(cpu->reg[H] == 0X0F);
    execute({ 0xCB, 0x35 });
    ASSERT(cpu->reg[L] == 0X0F);
    execute({ 0xCB, 0x37 });
    ASSERT(cpu->reg[A] == 0X0F);

    cpu->reg[H] = 0x80;
    mmu->set(cpu->reg16(HL), 0XF0);
    execute({ 0xCB, 0x36 });
    ASSERT(mmu->get(cpu->reg16(HL)) == 0X0F);

    init(0x00);
    execute({ 0xCB, 0x30 });
    ASSERT(cpu->reg[F] == 0x80);    // Test zero flag flag

    return true;
}

bool test_BIN_SRL()
{
    init(0b10001111);

    execute({ 0xCB, 0x38 });
    ASSERTV(cpu->reg[B] == 0b01000111, "value=%02X\n", cpu->reg[B]);
    execute({ 0xCB, 0x38 });
    ASSERT(cpu->reg[B] == 0b00100011);
    execute({ 0xCB, 0x39 });
    ASSERT(cpu->reg[C] == 0b01000111);
    execute({ 0xCB, 0x3A });
    ASSERT(cpu->reg[D] == 0b01000111);
    execute({ 0xCB, 0x3B });
    ASSERT(cpu->reg[E] == 0b01000111);
    execute({ 0xCB, 0x3C });
    ASSERT(cpu->reg[H] == 0b01000111);
    execute({ 0xCB, 0x3D });
    ASSERT(cpu->reg[L] == 0b01000111);
    execute({ 0xCB, 0x3F });
    ASSERT(cpu->reg[A] == 0b01000111);
    ASSERT(cpu->reg[F] == 0x10);    // Test carry flag

    cpu->reg[H] = 0x80;
    mmu->set(cpu->reg16(HL), 0b10001111);
    execute({ 0xCB, 0x3E });
    ASSERT(mmu->get(cpu->reg16(HL)) == 0b01000111);

    init(0b00000001);
    execute({ 0xCB, 0x38 });
    ASSERT(cpu->reg[F] == 0x90);    // Test zero flag flag

    return true;
}

bool test_BIN_BIT()
{
    for (uint8_t offset=0; offset<64; offset++) {
        unsigned char command = 0x40 + offset;

        /* (HL) special case */
        if (offset % 8 == 6) {
            init(0x88);
            mmu->set(0x8888, 0xFF);
        } else {
            init(0xFF);
        }

        execute({ 0xCB, command });
        ASSERTV((cpu->reg[F] & 0xF0) == 0x20, "F: 0x%02X opcode: 0x%02X\n", cpu->reg[F], command);

        /* (HL) special case */
        if (offset % 8 == 6) {
            init(0x88);
            mmu->set(0x8888, 0x00);
        } else {
            init(0x00);
        }

        execute({ 0xCB, command });
        ASSERTV((cpu->reg[F] & 0xF0) == 0xA0, "00 value=%02X offset=%d\n", cpu->reg[F], offset);
    }

    return true;
}

bool test_BIN_RES()
{
    for (uint8_t offset=0; offset<64; offset++) {
        unsigned char command = 0x80 + offset;
        init(0x55);
        execute({ 0xCB, command });

        uint8_t l_address[] = {
            cpu->reg[B],
            cpu->reg[C],
            cpu->reg[D],
            cpu->reg[E],
            cpu->reg[H],
            cpu->reg[L],
            mmu->get(cpu->reg16(HL)),
            cpu->reg[A],
        };

        uint8_t value = l_address[offset % 8];
        uint8_t mask = 1 << (offset / 8);

        ASSERTV((value & mask) == 0, "value=%02X mask=%02X offset=%d\n", value, mask, offset);
    }

    return true;
}

bool test_BIN_SET()
{
    for (uint8_t offset=0; offset<64; offset++) {
        unsigned char command = 0xC0 + offset;
        init(0x55);
        execute({ 0xCB, command });

        uint8_t l_address[] = {
            cpu->reg[B],
            cpu->reg[C],
            cpu->reg[D],
            cpu->reg[E],
            cpu->reg[H],
            cpu->reg[L],
            mmu->get(cpu->reg16(HL)),
            cpu->reg[A],
        };

        uint8_t value = l_address[offset % 8];
        uint8_t mask = 1 << (offset / 8);

        ASSERTV((value & mask) > 0, "value=%02X mask=%02X offset=%d\n", value, mask, offset);
    }

    return true;
}


/****************************************************************
 *
 *      TEST BOOT OPERATIONS
 *
 ****************************************************************/

/**
 * @brief      Zero the memory from $8000-$9FFF (VRAM)
 */
bool test_zero_memory()
{
    init(0x00);

    // Load fake data that should be erased
    uint16_t start = 0x8000;
    uint16_t end = 0x9FFF;
    for (size_t i=start; i<=end; i++) {
        mmu->set(i, 0xFF);
    }

    //cpu->display_registers();
    //mmu->dump(start, end);

    // Program to zero the memory
    size_t size = 12;
    uint8_t program[] = {
        0x31, 0xFE, 0xFF,
        0xAF,
        0x21, 0xFF, 0x9F,
        0x32,
        0xCB, 0x7C,
        0x20, 0xFB,
    };
    mmu->load(program, size);

    while (cpu->PC < size) {
        cpu->step();
    }

    //cpu->display_registers();
    //mmu->dump(start, end);

    bool success = true;
    for (size_t i=start; i<=end; i++) {
        success &= mmu->get(i) == 0x00;
    }
    ASSERT(success);

    return true;
}

/**
 * @brief      Test audio init
 */
bool test_audio_init()
{
    init(0x00);

    size_t size = 17;
    uint8_t program[] = {
        0x21, 0x26, 0xFF,   // LD HL,$ff26
        0x0E, 0x11,         // LD C,$11
        0x3E, 0x80,         // LD A,$80
        0x32,               // LD (HL-),A
        0xE2,               // LD ($FF00+C),A
        0x0C,               // INC C
        0x3E, 0xF3,         // LD A,$f3
        0xE2,               // LD ($FF00+C),A
        0x32,               // LD (HL-),A
        0x3E, 0x77,         // LD A,$77
        0x77,               // LD (HL),A

    };
    mmu->load(program, size);

    cpu->step();
    ASSERT(cpu->PC == 3);
    cpu->step();
    ASSERT(cpu->PC == 5);
    cpu->step();
    ASSERT(cpu->PC == 7);
    cpu->step();
    ASSERT(cpu->PC == 8);
    cpu->step();
    ASSERT(cpu->PC == 9);
    cpu->step();
    ASSERT(cpu->PC == 10);
    cpu->step();
    ASSERT(cpu->PC == 12);
    cpu->step();
    ASSERT(cpu->PC == 13);
    cpu->step();
    ASSERT(cpu->PC == 14);

    cpu->step();
    ASSERT(cpu->PC == 16);

    cpu->step();
    ASSERT(cpu->PC == 17);

    ASSERTV(cpu->reg16(HL) == 0xFF24, "HL should be 0xFF24: 0x%04X\n", cpu->reg16(HL));
    ASSERTV(cpu->reg[C] == 0x12, "C: 0x%02X\n", cpu->reg[C]);
    ASSERTV(cpu->reg[A] == 0x77, "A: 0x%02X\n", cpu->reg[A]);

    ASSERT(mmu->get(0xFF24) == 0x77);
    ASSERT(mmu->get(0xFF25) == 0xF3);
    ASSERT(mmu->get(0xFF26) == 0x80);

    return true;
}

bool test_graphic_routine()
{
    init(0x00);

    size_t size = 34;
    uint8_t program[] = {
        0x3E, 0xFC,
        0xE0, 0x47,
        0x11, 0x04, 0x01,       // LD DE, 0x0104
        0x21, 0x10, 0x80,       // LD HL, 0x8010
        0x1A,                   // LD A, (DE)
        0xCD, 0x0F, 0x00,       // CALL
        0x00,                   // NOP
        0x4f,                   // LD C, A
        0x06, 0x04,             // LD B, 0x04
        0xc5,                   // PUSH BC
        0xcb, 0x11,             // RL C
        0x17,                   // RLA
        0xc1,                   // POP BC
        0xcb, 0x11,             // RL C
        0x17,                   // RLA
        0x05,                   // DEC B
        0x20, 0xF5,             // JR NZ,
        0x22,                   // LD (HL+), A
        0x23,                   // INC HL
        0x22,                   // LD (HL+), A
        0x23,                   // INC HL
        0xc9,                   // RET
    };
    mmu->load(program, size);

    //mmu->dump(0x0000, 0x00FF);
    //cpu->display_registers();

    cpu->step();
    cpu->step();
    cpu->step();
    cpu->step();
    cpu->step();
    cpu->step();
    ASSERTV(cpu->reg[A] == 0x00, "A: 0x%02X\n", cpu->reg[A]);
    ASSERT(cpu->PC == 0x0F);

    size_t opcount = 0;
    const size_t max_opcount = 100;
    while (cpu->PC != 14 && opcount < max_opcount) {
        cpu->step();
        opcount++;
    }

    ASSERT(opcount < max_opcount);

    return true;
}


/****************************************************************
 *
 *      TEST CARTRIDGE OPERATIONS
 *
 ****************************************************************/

bool test_CARTRIDGE_post_boot()
{
    init(0x00);

    Cartridge cart;
    ASSERT(cart.load("tests/fake_rom.gb"));

    uint8_t program[] = { 0x00 };
    mmu->load(program, 1);

    mmu->set_cartridge(&cart);

    ASSERTV(mmu->get(0x0000) == 0x00, "0x0000: 0x%02X\n", mmu->get(0x0000));

    mmu->set(BOOT_ROM_ENABLE, 0x01);

    ASSERT(mmu->get(0x0000) == 0x87);

    return true;
}

bool test_CARTRIDGE_read_MBC1()
{
    init(0x00);

    Cartridge cart;
    ASSERT(cart.load("tests/fake_rom.gb"));

    mmu->set_cartridge(&cart);
    ASSERT(*(uint8_t *)cart.at(0x0104) == 0xCE);

    ASSERT(mmu->get(0x0104) == 0xCE);

    cpu->reg[D] = 0x01;
    cpu->reg[E] = 0x04;
    execute({ 0x1A });      // LD A,(DE)
    ASSERTV(cpu->reg[A] == 0xCE, "A: 0x%02X\n", cpu->reg[A]);

    return true;
}

/**
 * @brief      Loads a test cartridge cpu_instrs.gb and does some check
 */
bool test_CARTRIDGE_CPU_instrs()
{
    init(0x00);

    Cartridge cart;
    ASSERT(cart.load("tests/blargg/cpu_instrs.gb"));

    mmu->set_cartridge(&cart);

    cpu->PC = 0x0100;
    cpu->step();
    cpu->step();
    cpu->step();
    cpu->step();
    cpu->step();
    cpu->step();
    cpu->step();
    cpu->step();
    cpu->step();
    cpu->step();
    cpu->step();
    cpu->step();
    cpu->step();
    cpu->step();
    cpu->step();
    cpu->step();
    cpu->step();
    cpu->step();
    cpu->step();
    cpu->step();
    cpu->step();

    ASSERTV(cpu->PC == 0x0456, "PC: %04X\n", cpu->PC);

    cpu->step();

    //cpu->display_registers();
    //mmu->dump(0xDF00, 0xDFFF);

    cpu->step();
    cpu->step();
    cpu->step();
    cpu->step();
    cpu->step();
    cpu->step();
    cpu->step();
    cpu->step();

    //cpu->display_registers();
    //mmu->dump(0xDF00, 0xDFFF);

    ASSERTV(cpu->PC == 0x0459, "PC: %04X\n", cpu->PC);

    int i =0;
    while(i < 32700 && cpu->PC != 0x021D) {
        cpu->step(); i++;
    }

    cpu->step();

    ASSERTV(cpu->PC == 0x0228, "i: %d PC: %04X\n", i, cpu->PC);

    return true;
}

bool test_blargg_cpu_instrs()
{
    // Blargg cpu instrs
    const size_t blargg_count = 2;
    const char *cpu_instrs[] = {
        /*"tests/blargg/01-special.gb",
        "tests/blargg/02-interrupts.gb",
        "tests/blargg/03-op sp,hl.gb",
        "tests/blargg/04-op r,imm.gb",
        "tests/blargg/05-op rp.gb",
        "tests/blargg/06-ld r,r.gb",
        "tests/blargg/07-jr,jp,call,ret,rst.gb",
        "tests/blargg/08-misc instrs.gb",
        "tests/blargg/09-op r,r.gb",
        "tests/blargg/10-bit ops.gb",
        "tests/blargg/11-op a,(hl).gb",*/
        "tests/blargg/cpu_instrs.gb",
        "tests/blargg/instr_timing.gb",
    };

    for (size_t test_id=0; test_id<blargg_count; test_id++) {
        const char *path_rom = cpu_instrs[test_id];
        fprintf(stdout, "BLARGG: %s: ", path_rom);

        DMG dmg;
        dmg.init("bios/dmg_boot.bin", cpu_instrs[test_id]);
        dmg.set_palette('1');
        dmg.set_speed(5000);
        dmg.run();

        fprintf(stdout, "\n");
    }

    return true;
}

int main(void)
{
    cart = new Cartridge();
    mmu = new MMU();
    cpu = new CPU();
    ppu = new PPU();
    timer = new Timer();
    input = new Input();

    mmu->set_ppu(ppu);
    mmu->set_timer(timer);
    mmu->set_input(input);
    cpu->set_mmu(mmu);
    ppu->set_mmu(mmu);
    timer->set_mmu(mmu);
    input->set_mmu(mmu);

    mmu->init(nullptr, cart);
    cpu->init();
    ppu->init();
    timer->init();
    input->init();

    fprintf(stdout, "DMG auto testing\n");

    test("MMU: RAM check", &test_MMU_ram);

    test("Binary Operations: RLC", &test_BIN_RLC);
    test("Binary Operations: RRC", &test_BIN_RRC);
    test("Binary Operations: RL", &test_BIN_RL);
    test("Binary Operations: RR", &test_BIN_RR);
    test("Binary Operations: SLA", &test_BIN_SLA);
    test("Binary Operations: SRA", &test_BIN_SRA);
    test("Binary Operations: SWAP", &test_BIN_SWAP);
    test("Binary Operations: SRL", &test_BIN_SRL);
    test("Binary Operations: BIT", &test_BIN_BIT);
    test("Binary Operations: RES", &test_BIN_RES);
    test("Binary Operations: SET", &test_BIN_SET);

    test("CPU: NOP", &test_CPU_NOP);
    test("CPU: LD 8-Bit", &test_CPU_LD_8bit);
    test("CPU: LD 16-Bit", &test_CPU_LD_16bit);
    test("CPU: INC", &test_CPU_INC);
    test("CPU: ADD", &test_CPU_ADD);
    test("CPU: SUB", &test_CPU_SUB);
    test("CPU: XOR", &test_CPU_XOR);
    test("CPU: OR", &test_CPU_OR);
    test("CPU: AND", &test_CPU_AND);
    test("CPU: CP", &test_CPU_CP);
    test("CPU: CALL/RET", &test_CPU_CALL_RET);
    test("CPU: RST", &test_CPU_RST);
    test("CPU: PUSH/POP", &test_CPU_PUSH_POP);
    test("CPU: RLA/RLCA", &test_CPU_RLA_RLCA);
    test("CPU: INC/DEC", &test_CPU_INC_DEC);
    test("CPU: JP", &test_CPU_JP);
    test("CPU: POP AF", &test_CPU_POP_AF);
    test("CPU: DAA", &test_CPU_DAA);
    test("CPU: Op SP", &test_CPU_Op_SP);

    test("PROGRAM: Zero memory from $8000 to $9FFF", &test_zero_memory);
    test("PROGRAM: Init sound control registers", &test_audio_init);
    test("PROGRAM: Boot graphic routine", &test_graphic_routine);

    test("CARTRIDGE: Post boot", &test_CARTRIDGE_post_boot);
    test("CARTRIDGE: Read from MBC1", &test_CARTRIDGE_read_MBC1);
    test("CARTRIDGE: CPU Instrs", &test_CARTRIDGE_CPU_instrs);

    delete(cart);
    delete(mmu);
    delete(cpu);
    delete(ppu);
    delete(timer);
    delete(input);

    test_blargg_cpu_instrs();

    return EXIT_SUCCESS;
}
