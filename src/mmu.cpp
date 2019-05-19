#include <stdio.h>
#include <string.h>

#include "utils.h"
#include "log.h"
#include "ppu.h"

#include "mmu.h"


MMU::MMU() : booted(false), cart(nullptr)
{
    ppu = nullptr;
}


void MMU::set_ppu(PPU *ppu)
{
    this->ppu = ppu;
}


/**
 * @brief      Get type of address controller at the specified address
 * Used to determine if we can read/write and map request
 * @param[in]  address  The address
 * @return     The address identity.
 */
address_type MMU::get_address_identity(uint16_t address)
{
    // Assume BOOT and ROM memroies both starts at 0x0000

    // boot ROM
    if (!booted && /*address >= BOOT_START &&*/ address <= BOOT_END) {
        return BOOT;
    }

    // Cartridge ROM
    if (/*address >= ROM_START &&*/ address <= ROM_END) {
        return ROM;
    }

    // ECHO
    if (address >= ECHO_START && address <= ECHO_END) {
        return ECHO;
    }

    // CRASH
    if (address >= CRASH_START && address <= CRASH_END) {
        return CRASH;
    }

    return RAM;
}


/**
 * @brief      Set the value at the desired address
 * @param[in]  address  The address
 * @param[in]  value    The value to write
 * @return     false if the address designed a READ only memory
 */
bool MMU::set(uint16_t address, uint8_t value)
{
#ifdef DEBUG
    if (ppu == nullptr) {
        error("PPU is not linked to the MMU: Segfault will happen. Use mmu->set_ppu()\n");
    }
#endif
    address_type identity = get_address_identity(address);
    //debug("set 0x%04X 0x%02X\n", address, value);

    // Cannot write over BOOT rom
    if (identity == BOOT) {
        return false;
    }

    // Write to ROM are passed to cartridge
    else if (identity == ROM) {
        return cart->set(address, value);
    }

    // Writting to CRASH causes errors
    else if (identity == CRASH) {
        // TODO: Crash?
        error("Trying to write to CRASH memory\n");
        return false;
    }

    // ECHO memory
    else if (identity == ECHO) {
        address -= 0x2000;
    }

    ram[address] = value;

    // BOOT Status
    if (address == BOOT_ROM_ENABLE) {
        set_boot_rom_enable(value);
    }

    // LCD Control
    else if (address == LCDC) {
        ppu->set_lcdc(value);
    }

    // BG Palette
    else if (address == BGP) {
        ppu->set_bgp(value);
    }

    return false;
}


/**
 * @brief      Give the emulated address of the given DMG address
 * @param[in]  address  The address in the DMG memory
 * @return     Pointer to the immutable value
 */
const void *MMU::at(uint16_t address)
{
    address_type identity = get_address_identity(address);
    if (identity == ROM) {
        if (cart != nullptr) {
            return cart->at(address);
        }
    }

    // ECHO memory
    else if (identity == ECHO) {
        address -= 0x2000;
    }

    return ram + address;
}


uint8_t MMU::get(uint16_t address)
{
    return *(uint8_t*) at(address);
}


uint16_t MMU::get16(uint16_t address)
{
    uint16_t low = (uint16_t)get(address);
    uint16_t high = (uint16_t)get(address + 1);

    return (high << 8) + low;
}


int8_t MMU::get_signed(uint16_t address)
{
    return get(address);
}


/**
 * @brief      Loads boot ROM
 * @param[in]  path_rom     Where the ROM is
 * @return     { description_of_the_return_value }
 */
bool MMU::load(const char *path_rom)
{
    debug("Loading boot ROM: %s\n", path_rom);

    FILE *f = fopen(path_rom, "rb");
    if (f == NULL) {
        error("Unable to read provided boot ROM file\n");
        return false;
    }

    uint8_t byte;
    uint16_t dst = BOOT_START;
    while (fread(&byte, sizeof(uint8_t), 1, f) == 1) {
        ram[dst++] = byte;
    }

    // Failure of reading not EOF related
    if (!feof(f)) {
        error("Error while reading provided boot ROM file\n");
        return false;
    }

    return true;
}


bool MMU::load(uint8_t *program, size_t size, uint16_t dst)
{
    for (size_t i=0; i<size; i++) {
        ram[dst + i] = program[i];
    }

    return true;
}


bool MMU::load(uint8_t *program, size_t size)
{
    return load(program, size, 0x0000);
}


/**
 * @brief      Plugs a cartridge inside the memory
 * @param      cart  Cartridge to be used
 */
void MMU::set_cartridge(Cartridge *cart)
{
    this->cart = cart;
}


/**
 * @brief      Allows to control if BOOT tom is readable or not
 * @param[in]  value  The value 0x01 indicate boot is no longer needed
 */
void MMU::set_boot_rom_enable(uint8_t value)
{
    if (value & 0x01) {
        debug("BOOT sequence over\n");
        booted = true;
    } else {
        booted = false;
    }
}


bool MMU::is_booted()
{
    return booted;
}
