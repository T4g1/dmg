#include "none.h"

#include <string.h>

#include "../defines.h"
#include "../log.h"


NoMBC::NoMBC()
{

}


NoMBC::~NoMBC()
{
    delete memory;
    delete ram;
}


void NoMBC::init()
{
    memory = new uint8_t[ROM_SIZE];
    ram = new uint8_t[RAM_MBC_SIZE];
}


uint8_t NoMBC::get(uint16_t address)
{
    if (address <= ROM1_END) {
        return memory[address];
    } else if (address >= SRAM_START && address <= SRAM_END) {
        return ram[address % RAM_MBC_SIZE];
    } else {
        return 0;
    }
}


bool NoMBC::load(size_t mb_index, const uint8_t *rom)
{
    memcpy(memory + (MBC_SIZE * mb_index), rom, MBC_SIZE);

    return true;
}


bool NoMBC::set(uint16_t address, uint8_t value)
{
    if (address >= SRAM_START && address <= SRAM_END) {
        ram[address] = value;
    }

    return true;
}
