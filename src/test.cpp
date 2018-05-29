#include "test.h"

#include "cpu.h"
#include "mmu.h"

MMU *mmu;
CPU *cpu;

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
    cpu->PC = 0;

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
    cpu->PC = 0;
    cpu->reg[F] = 0x00;
    cpu->reg[B] = 0xF0;
    cpu->reg[C] = 0xF0;
    cpu->reg[D] = 0xF0;
    cpu->reg[E] = 0xF0;
    cpu->reg[H] = 0xF0;
    cpu->reg[L] = 0xF0;
    cpu->reg[A] = 0xF0;

    uint8_t program[] = {
        0xCB, 0x00,         // RLC B
        0xCB, 0x00,         // RLC B
        0xCB, 0x01,         // RLC C
        0xCB, 0x02,         // RLC D
        0xCB, 0x03,         // RLC E
        0xCB, 0x04,         // RLC H
        0xCB, 0x05,         // RLC L
        0xCB, 0x06,         // RLC (HL)
        0xCB, 0x07          // RLC A
    };
    mmu->load(program, 18);

    cpu->step();
    ASSERTV(cpu->reg[B] == 0b11100001, "F=%02X B=%02X\n", cpu->reg[F], cpu->reg[B]);
    ASSERTV(cpu->reg[F] == 0x10, "F=%02X B=%02X\n", cpu->reg[F], cpu->reg[B]);
    cpu->step();
    ASSERTV(cpu->reg[B] == 0b11000011, "F=%02X B=%02X\n", cpu->reg[F], cpu->reg[B]);
    cpu->step();
    ASSERT(cpu->reg[C] == 0b11100001);
    cpu->step();
    ASSERT(cpu->reg[D] == 0b11100001);
    cpu->step();
    ASSERT(cpu->reg[E] == 0b11100001);
    cpu->step();
    ASSERT(cpu->reg[H] == 0b11100001);
    cpu->step();
    ASSERT(cpu->reg[L] == 0b11100001);
    mmu->set(cpu->reg16(HL), 0xF0);
    cpu->step();
    ASSERT(mmu->get(cpu->reg16(HL)) == 0b11100001);
    cpu->step();
    ASSERT(cpu->reg[A] == 0b11100001);

    return true;
}

int main(void)
{
    mmu = new MMU();
    cpu = new CPU(mmu);

    test("MMU: RAM check", &test_MMU_ram);
    test("Binary Operations: RLC", &test_BIN_RLC);
    test("CPU: NOP", &test_CPU_NOP);
    test("CPU: LD 16-Bit", &test_CPU_LD_16bit);

    return EXIT_SUCCESS;
}
