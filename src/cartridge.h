#ifndef CARTRIDGE_H
#define CARTRIDGE_H

#include <stdint.h>

#include "mbc.h"

#define CARTRIDGE_TYPE_ADDRESS      0x0147

#define CART_TYPE_ROM_ONLY      0x00
#define CART_TYPE_MBC1          0x01


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
};

#endif /* CARTRIDGE_H */
