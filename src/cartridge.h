#ifndef CARTRIDGE_H
#define CARTRIDGE_H

#include <iostream>
#include <iostream>
#include <fstream>

#include "mbc.h"


/**
 * @brief      DMG Emulator
 */
class Cartridge {
public:
    MBC *mbc;
    uint8_t cartridge_type;
    uint8_t rom_type;
    uint8_t ram_type;

    std::string rom_path;
    bool has_battery;

    Cartridge();
    ~Cartridge();

    void clear_mbc();

    bool load(std::string rom_path);
    uint8_t get(uint16_t address);
    bool set(uint16_t address, uint8_t value);

    bool has_ram();

    std::string get_save_name();
    void save_ram();
    void load_ram();

    static size_t get_rom_bank_count(uint8_t rom_type);
    static size_t get_ram_bank_count(uint8_t ram_type);
    static size_t get_ram_size(uint8_t ram_type);

    void serialize(std::ofstream &file);
    void deserialize(std::ifstream &file);
};

#endif /* CARTRIDGE_H */
