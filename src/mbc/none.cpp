#include "none.h"

#include <string.h>

#include "../log.h"


NoMBC::NoMBC()
{

}


void NoMBC::init()
{
    memory = new uint8_t[ROM_SIZE];
}


uint8_t NoMBC::get(uint16_t address)
{
    return memory[address % ROM_SIZE];
}


bool NoMBC::load(size_t mb_index, const uint8_t *rom)
{
    memcpy(memory + (MBC_SIZE * mb_index), rom, MBC_SIZE);

    return true;
}


bool NoMBC::set(uint16_t /*address*/, uint8_t /*value*/)
{
    // Nothing to do, there is no MBC
    return true;
}
