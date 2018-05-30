#include "test.h"

#include <initializer_list>

#include "cpu.h"
#include "mmu.h"

MMU *mmu;
CPU *cpu;

/**
 * @brief      Init CPU to PC 0 and same value for all register except F
 * @param[in]  value  The value for all registers
 */
void init(uint8_t value)
{
    cpu->PC = 0x00;

    cpu->reg[F] = 0x00;

    cpu->reg[B] = value;
    cpu->reg[C] = value;
    cpu->reg[D] = value;
    cpu->reg[E] = value;
    cpu->reg[H] = value;
    cpu->reg[L] = value;
    cpu->reg[A] = value;
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

    mmu->set(0x0000, 0x00);
    mmu->set(0x0001, 0xDD);
    mmu->set(0x0002, 0xFF);
    mmu->set(0x0003, 0x12);

    ASSERT(mmu->get(0x0000) == 0x00);
    ASSERT(mmu->get(0x0001) == 0xDD);
    ASSERT(mmu->get(0x0002) == 0xFF);
    ASSERT(mmu->get(0x0003) == 0x12);

    size_t size = 3;
    uint8_t program[] = { 0x01, 0x00, 0x11 };
    mmu->load(program, size);
    for (size_t i=0; i<size; i++) {
        ASSERT(mmu->get(i) == program[i]);
    }

    return true;
}

bool test_CPU_NOP()
{
    // NOP
    cpu->PC = 0;
    mmu->set(0, 0x00);

    ASSERT(cpu->PC == 0);
    cpu->step();
    ASSERT(cpu->PC == 1);

    return true;
}

bool test_CPU_LD_16bit()
{
    init(0x00);

    uint8_t program[] = {
        0x01, 0x00, 0x11,   // LD BC, d16
        0x11, 0x22, 0x33,   // LD DE, d16
        0x21, 0x44, 0x55,   // LD HL, d16
        0x31, 0x66, 0x77,   // LD SP, d16
        0x08, 0x66, 0x77,   // LD (a16), SP
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
    cpu->step();
    ASSERT(cpu->PC == 15);

    ASSERT(cpu->reg16(BC) == 0x1100);
    ASSERT(cpu->reg16(DE) == 0x3322);
    ASSERT(cpu->reg16(HL) == 0x5544);
    ASSERTV(cpu->reg16(SP) == 0x7766, "SP=0x%04X", cpu->reg16(SP));
    ASSERT(mmu->get16(0x7766) == 0x7766);

    cpu->step();
    ASSERTV(cpu->PC == 16, "PC=0x%04X", cpu->PC);
    cpu->step();
    ASSERTV(cpu->PC == 18, "PC=0x%04X", cpu->PC);

    ASSERTV(cpu->reg16(SP) == 0x5544, "SP=0x%04X", cpu->reg16(SP));
    ASSERTV(cpu->reg16(HL) == 0x5544 + 0x14, "HL=0x%04X", cpu->reg16(HL));

    return true;
}

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
    execute({ 0xCB, 0x0E });
    ASSERT(mmu->get(cpu->reg16(HL)) == 0b10000111);

    // Test zero flag
    init(0x00);
    execute({ 0xCB, 0x18 });
    ASSERT(cpu->reg[F] == 0b10000000);

    return true;
}

int main(void)
{
    mmu = new MMU();
    cpu = new CPU(mmu);

    test("MMU: RAM check", &test_MMU_ram);
    test("Binary Operations: RLC", &test_BIN_RLC);
    test("Binary Operations: RRC", &test_BIN_RRC);
    test("Binary Operations: RL", &test_BIN_RL);
    test("Binary Operations: RR", &test_BIN_RR);
    test("CPU: NOP", &test_CPU_NOP);
    test("CPU: LD 16-Bit", &test_CPU_LD_16bit);

    return EXIT_SUCCESS;
}
