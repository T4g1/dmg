#ifndef CARTRIDGE_H
#define CARTRIDGE_H

#include "mbc.h"


/**
 * @brief      DMG Emulator
 */
class Cartridge {
public:
    MBC *mbc;

    Cartridge();

    bool load(const char *path_rom);
    uint8_t get(uint16_t address);
    bool set(uint16_t address, uint8_t value);

    static size_t get_rom_bank_count(uint8_t rom_type);
    static size_t get_ram_bank_count(uint8_t ram_type);
};

#endif /* CARTRIDGE_H */
