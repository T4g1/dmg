#include "mbc1.h"

#include <string.h>

#include "../mmu.h"
#include "../log.h"


MBC1::MBC1(size_t rom_mbc_count, uint8_t ram_type)
{
    debug("MBC1 ROM: %zu banks RAM: 0x%02X\n", rom_mbc_count, ram_type);
    this->rom_mbc_count = rom_mbc_count;
    this->ram_size = Cartridge::get_ram_size(ram_type);
    this->ram_mbc_count = Cartridge::get_ram_bank_count(ram_type);

    rom_mbc_count = 0;

    rom_mode_selected = true;
    ram_enabled = false;

    rom_bank_select = 0;
    ram_bank_select = 0;
}


MBC1::~MBC1()
{
    delete memory;
    delete ram;
}


void MBC1::init()
{
    memory = new uint8_t[rom_mbc_count * MBC_SIZE];
    ram = new uint8_t[ram_mbc_count * RAM_MBC_SIZE];
    ram[0] = 0x11;
    ram[1] = 0x22;
    ram[2] = 0x33;
}


/**
 * @brief      Give value from MBC
 * @param[in]  address  Requested address
 * @return     Address to an immutable value
 */
uint8_t MBC1::get(uint16_t address)
{
    if (address <= ROM0_END) {
        return memory[address % MBC_SIZE];
    } else if (address <= ROM1_END) {
        return memory[(get_selected_rom_bank() * MBC_SIZE) + (address % MBC_SIZE)];
    } else if (ram_enabled) {
        return ram[(get_selected_ram_bank() * RAM_MBC_SIZE) + (address % RAM_MBC_SIZE)];
    } else {
        return 0;
    }
}


/**
 * @brief      Stores the given 16k of ROM into the MBC at the given index
 * @param[in]  mb_index  The memory bank index
 * @param[in]  rom        The rom
 * @return     true if loaded with success
 */
bool MBC1::load(size_t mb_index, const uint8_t *rom)
{
    if (mb_index >= rom_mbc_count) {
        error("MBC1 have at most %zu memory bank(s)\n", rom_mbc_count);
        return false;
    }

    memcpy(memory + (mb_index * MBC_SIZE), rom, MBC_SIZE);

    return true;
}


bool MBC1::set(uint16_t address, uint8_t value)
{
    // Enable RAM
    if (address <= RAM_ENABLE_END) {
        ram_enabled = (value & 0x0F) == 0x0A;
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

    // Write to RAM
    else if (address >= SRAM_START && address <= SRAM_END) {
        ram[(get_selected_ram_bank() * RAM_MBC_SIZE) + (address % RAM_MBC_SIZE)] = value;
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


/**
 * @brief      Compute selected RAM bank
 * @return     The selected ram bank.
 */
size_t MBC1::get_selected_ram_bank()
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
