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
    ASSERT(cpu->reg[F] == 0x80);    // Test zero flag flag

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
            init(0x55);
            mmu->set(0x5555, 0xFF);
        } else {
            init(0xFF);
        }

        execute({ 0xCB, command });
        ASSERTV((cpu->reg[F] & 0xF0) == 0x20, "FF value=%02X offset=%d\n", cpu->reg[F], offset);

        /* (HL) special case */
        if (offset % 8 == 6) {
            init(0x55);
            mmu->set(0x5555, 0x00);
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

        uint8_t *l_address[] = {
            &cpu->reg[B],
            &cpu->reg[C],
            &cpu->reg[D],
            &cpu->reg[E],
            &cpu->reg[H],
            &cpu->reg[L],
            (uint8_t*) mmu->at(cpu->reg16(HL)),
            &cpu->reg[A],
        };

        uint8_t value = *l_address[offset % 8];
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

        uint8_t *l_address[] = {
            &cpu->reg[B],
            &cpu->reg[C],
            &cpu->reg[D],
            &cpu->reg[E],
            &cpu->reg[H],
            &cpu->reg[L],
            (uint8_t*) mmu->at(cpu->reg16(HL)),
            &cpu->reg[A],
        };

        uint8_t value = *l_address[offset % 8];
        uint8_t mask = 1 << (offset / 8);

        ASSERTV((value & mask) > 0, "value=%02X mask=%02X offset=%d\n", value, mask, offset);
    }

    return true;
}

bool test_boot_bios()
{
    init(0x00);
    execute({ 0x31, 0xFE, 0xFF });
    execute({ 0xAF });
    execute({ 0x21, 0xFF, 0x9F });

    cpu->display_registers();
    mmu->dump(0x0000, 0x00FF);

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
    test("Binary Operations: SLA", &test_BIN_SLA);
    test("Binary Operations: SRA", &test_BIN_SRA);
    test("Binary Operations: SWAP", &test_BIN_SWAP);
    test("Binary Operations: SRL", &test_BIN_SRL);
    test("Binary Operations: BIT", &test_BIN_BIT);
    test("Binary Operations: RES", &test_BIN_RES);
    test("Binary Operations: SET", &test_BIN_SET);
    test("CPU: NOP", &test_CPU_NOP);
    test("CPU: LD 16-Bit", &test_CPU_LD_16bit);

    test("Boot BIOS", &test_boot_bios);

    return EXIT_SUCCESS;
}
