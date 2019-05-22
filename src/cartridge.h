#ifndef CARTRIDGE_H
#define CARTRIDGE_H

#include <stdint.h>

#include "mbc.h"

#define CARTRIDGE_TYPE_ADDRESS          0x0147
#define ROM_SIZE_ADDRESS                0x0148
#define RAM_SIZE_ADDRESS                0x0149

#define CART_TYPE_ROM_ONLY              0x00
#define CART_TYPE_MBC1                  0x01
#define CART_TYPE_MBC1_RAM              0x02
#define CART_TYPE_MBC1_RAM_BATTERY      0x03


/**
 * @brief      DMG Emulator
 */
class Cartridge {
public:
    MBC *mbc;

    Cartridge();

    bool load(const char *path_rom);
    const void *at(uint16_t address);
    bool set(uint16_t address, uint8_t value);

    static size_t get_bank_count(uint8_t rom_type);
};

#endif /* CARTRIDGE_H */
