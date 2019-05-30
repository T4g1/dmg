#ifndef DMG_H
#define DMG_H

#include <iostream>

#include "ppu.h"
#include "cpu.h"
#include "input.h"
#include "timer.h"
#include "apu.h"
#include "gui/debugger.h"


/**
 * @brief      DMG Emulator
 */
class DMG {
    MMU *mmu;
    CPU *cpu;
    PPU *ppu;
    Input *input;
    Timer *timer;
    APU *apu;

    Debugger *debugger;

    bool running;
    bool no_boot;

    size_t system_clock;

    std::string bios_path;
    std::string rom_path;

    size_t palette;

public:
    size_t save_slot;

    ~DMG();

    bool init(const char *bios_path, const char *rom_path);
    int run();
    void process();
    void handle_events();
    void reset();

    void update_system_clock();

    void fake_boot();
    void set_palette(size_t palette_index);
    void set_speed(size_t speed);

    void load_rom(std::string filepath);

    std::string get_save_name();
    void select_prev_save_slot();
    void select_next_save_slot();
    void save_state();
    void load_state();

    friend class Debugger;
};

#endif /* DMG_H */
