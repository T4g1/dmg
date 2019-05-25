#ifndef MMU_H
#define MMU_H

#include <cstddef>

#include "defines.h"
#include "cartridge.h"


class PPU;
class Timer;
class Input;
class Debugger;


/**
 * @brief      Memory Map Unit
 */
class MMU {
    bool booted;
    Cartridge *cart;
    PPU *ppu;
    Timer *timer;
    Input *input;
    Debugger *debugger;

    uint8_t boot[BOOT_SIZE];
    uint8_t ram[RAM_SIZE];

    address_type get_address_identity(uint16_t address);

    uint8_t memory_masks(uint16_t address, uint8_t value);
    void handle_callbacks(uint16_t address, uint8_t value);

public:
    MMU();

    bool init(const char *path_bios, Cartridge *cart);
    void reset();

    bool set(uint16_t address, uint8_t value);
    void set_nocheck(uint16_t address, uint8_t value);
    uint8_t get(uint16_t address);
    uint8_t get_nocheck(uint16_t address);
    uint16_t get16(uint16_t address);
    int8_t get_signed(uint16_t address);

    bool load(const char *filepath);
    bool load(uint8_t *program, size_t size, uint16_t dst);
    bool load(uint8_t *program, size_t size);

    void set_cartridge(Cartridge *cart);
    void set_boot_rom_enable(uint8_t value);

    void set_booted(bool value);
    bool is_booted();

    void update_ram();
    void trigger_interrupt(uint8_t interrupt_mask);

    const char *display_address_identity(uint16_t address);

    void set_ppu(PPU *ppu);
    void set_timer(Timer *timer);
    void set_input(Input *input);
    void set_debugger(Debugger *debugger);

    friend class Debugger;
};

#endif /* MMU_H */
