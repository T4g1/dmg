#ifndef MMU_H
#define MMU_H

#include <cstddef>
#include <iostream>
#include <fstream>

#include "defines.h"
#include "cartridge.h"


class PPU;
class Timer;
class Input;
class APU;
class Debugger;


/**
 * @brief      Memory Map Unit
 */
class MMU {
    Cartridge *cart;
    PPU *ppu;
    Timer *timer;
    Input *input;
    APU *apu;
    Debugger *debugger;

    bool booted;
    uint8_t boot[BOOT_SIZE];
    uint8_t ram[RAM_SIZE];

    address_type get_address_identity(uint16_t address);

    uint8_t memory_masks(uint16_t address, uint8_t value);
    void handle_callbacks(uint16_t address, uint8_t value);

public:
    bool no_boot;           // No boot rom provided

    MMU();
    ~MMU();

    bool init(std::string boot_path, std::string rom_path);
    void reset();

    bool set(uint16_t address, uint8_t value);
    void set_nocheck(uint16_t address, uint8_t value);
    uint8_t get(uint16_t address);
    uint8_t get_nocheck(uint16_t address);
    uint16_t get16(uint16_t address);
    int8_t get_signed(uint16_t address);

    bool load_boot(std::string filepath);
    bool load_rom(std::string filepath);
    bool load(uint8_t *program, size_t size, uint16_t dst);
    bool load(uint8_t *program, size_t size);

    void set_boot_rom_enable(uint8_t value);

    void set_booted(bool value);
    bool is_booted();

    void trigger_interrupt(uint8_t interrupt_mask);

    const char *display_address_identity(uint16_t address);

    void set_ppu(PPU *ppu);
    void set_timer(Timer *timer);
    void set_input(Input *input);
    void set_apu(APU *apu);
    void set_debugger(Debugger *debugger);

    void serialize(std::ofstream &file);
    void deserialize(std::ifstream &file);

    friend class Debugger;
};

#endif /* MMU_H */
