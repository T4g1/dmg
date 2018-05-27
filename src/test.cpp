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

    return true;
}

bool test_CPU_NOP()
{
    mmu->set(0x0000, 0x00);

    ASSERT(cpu->PC == 0);
    cpu->step();
    ASSERT(cpu->PC == 1);

    return true;
}


int main(void)
{
    mmu = new MMU();
    cpu = new CPU(mmu);

    test("MMU: RAM check", &test_MMU_ram);
    test("CPU: NOP", &test_CPU_NOP);

    return EXIT_SUCCESS;
}
