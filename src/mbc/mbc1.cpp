#include "mbc1.h"

#include <string.h>

#include "../mmu.h"
#include "../log.h"


MBC1::MBC1() : selected_mbc(1)
{

}


void MBC1::init()
{
    memory = new uint8_t[MBC_COUNT * MBC_SIZE];
}


/**
 * @brief      Give access to the requested address in the correct memory bank
 * @param[in]  address  Requested address
 * @return     Address to an immutable value
 */
const void *MBC1::at(uint16_t address)
{
    if (address <= MBC0_END) {
        return &memory[address % MBC_SIZE];
    }

    return &memory[(selected_mbc * MBC_SIZE) + (address % MBC_SIZE)];
}


/**
 * @brief      Stores the given 16k of ROM into the MBC at the given index
 * @param[in]  mb_index  The memory bank index
 * @param[in]  rom        The rom
 * @return     true if loaded with success
 */
bool MBC1::load(size_t mb_index, const uint8_t *rom)
{
    if (mb_index >= MBC_COUNT) {
        error("MBC1 have at most %d memory bank(s)\n", MBC_COUNT);
        return false;
    }

    memcpy(memory + (mb_index * MBC_SIZE), rom, MBC_SIZE);

    return true;
}


bool MBC1::set(uint16_t /*address*/, uint8_t value)
{
    // Selecting memory bank 0 gives bank 1
    // Selecting memory bank above MBC_COUNT wraps
    value = value % MBC_COUNT;
    if (value == 0) {
        value = 1;
    }

    debug("Set MBC to 0x%02X\n", value);
    selected_mbc = value;

    return true;
}
