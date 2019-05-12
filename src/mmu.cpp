#include <stdio.h>
#include <string.h>

#include "utils.h"
#include "log.h"

#include "mmu.h"


MMU::MMU() : booted(false), cart(nullptr)
{

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

    // BOOT ROM enable refister
    if (address == BOOT_ROM_ENABLE) {
        return REG_BOOT_ROM_ENABLE;
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
    address_type identity = get_address_identity(address);

    if (identity == RAM) {
        ram[address] = value;
        return true;
    } else if (identity == REG_BOOT_ROM_ENABLE) {
        if (value & 0x01) {
            booted = true;
        } else {
            booted = false;
        }
    }

    // TODO: Request WRITE to corresponding ROM to trigger bank switch for example

    return false;
}


/**
 * @brief      Give the emulated address of the given DMG address
 * @param[in]  address  The address in the DMG memory
 * @return     Pointer to the immutable value
 */
const void *MMU::at(uint16_t address)
{
    if (get_address_identity(address) == ROM) {
        if (cart != nullptr) {
            return cart->at(address);
        }
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
 * @brief      Display memory status from given address ranges
 * @param[in]  start  Starting point of the dump
 * @param[in]  end    End point of the dump
 */
void MMU::dump(uint16_t start, uint16_t end)
{
    info("---------------------------[ MMU ]---------------------------\n");
    size_t width = 16;

    for (size_t i=start; i<=end; i++) {
        if (i % width == 0) {
            info("0x%04X-0x%04X ", (int)i, (int)(i + width - 1));
        }
        info("%02X ", get(i));

        if (i % width == width - 1) {
            info("\n");
        }
    }
    info("\n");
}


/**
 * @brief      Plugs a cartridge inside the memory
 * @param      cart  Cartridge to be used
 */
void MMU::set_cartridge(Cartridge *cart)
{
    this->cart = cart;
}
