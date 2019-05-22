#include "mbc1.h"

#include <string.h>

#include "../mmu.h"
#include "../log.h"


MBC1::MBC1(size_t mbc_count)
{
    debug("MBC1 RAM: %zu banks\n", mbc_count);
    this->mbc_count = mbc_count;

    mbc_count = 0;

    rom_mode_selected = true;

    rom_bank_select = 0;
    ram_bank_select = 0;
}


MBC1::~MBC1()
{
    delete memory;
}


void MBC1::init()
{
    memory = new uint8_t[mbc_count * MBC_SIZE];
}


/**
 * @brief      Give access to the requested address in the correct memory bank
 * @param[in]  address  Requested address
 * @return     Address to an immutable value
 */
const void *MBC1::at(uint16_t address)
{
    if (address <= ROM0_END) {
        return &memory[address % MBC_SIZE];
    }

    return &memory[(get_selected_rom_bank() * MBC_SIZE) + (address % MBC_SIZE)];
}


/**
 * @brief      Stores the given 16k of ROM into the MBC at the given index
 * @param[in]  mb_index  The memory bank index
 * @param[in]  rom        The rom
 * @return     true if loaded with success
 */
bool MBC1::load(size_t mb_index, const uint8_t *rom)
{
    if (mb_index >= mbc_count) {
        error("MBC1 have at most %zu memory bank(s)\n", mbc_count);
        return false;
    }

    memcpy(memory + (mb_index * MBC_SIZE), rom, MBC_SIZE);

    return true;
}


bool MBC1::set(uint16_t address, uint8_t value)
{
    // Enable RAM
    if (address <= RAM_ENABLE_END) {
        // TODO
    }

    // ROM bank select
    else if (address <= ROM_BANK_NUMBER_END) {
        rom_bank_select = value & LOWER_ROM_BANK_NUMBER_MASK;
    }

    // RAM bank select
    else if (address <= RAM_BANK_NUMBER_END) {
        ram_bank_select = value & UPPER_ROM_BANK_NUMBER_MASK;
    }

    // ROM/RAM mode selector
    else if (address <= ROM_RAM_MODE_SELECT_END) {
        if (value == 0x00) {
            rom_mode_selected = true;
        }

        if (value == 0x01) {
            rom_mode_selected = false;
        }
    }

    return true;
}


/**
 * @brief      Compute selected ROM bank
 * @return     The selected rom bank.
 */
size_t MBC1::get_selected_rom_bank()
{
    size_t lower = rom_bank_select;
    if (lower == 0) {
        lower = 1;
    }

    size_t higher = 0;
    if (rom_mode_selected) {
        higher = ram_bank_select;
    }

    return (higher << 5) + lower;
}
