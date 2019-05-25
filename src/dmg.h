#ifndef DMG_H
#define DMG_H

#include "ppu.h"
#include "cpu.h"
#include "input.h"
#include "timer.h"
#include "cartridge.h"
#include "gui/debugger.h"


void quit();


/**
 * @brief      DMG Emulator
 */
class DMG {
    Cartridge cart;
    MMU *mmu;
    CPU *cpu;
    PPU *ppu;
    Input *input;
    Timer *timer;

    Debugger *debugger;

    bool running;
    bool no_boot;

    size_t system_clock;

public:
    bool init(const char *path_bios, const char *path_rom);
    int run();
    void process();
    void handle_events();
    void reset();

    void fake_boot();
    void set_palette(char palette_index);
    void set_speed(size_t speed);

    void save_state();
    void load_state();

    friend class Debugger;
};

#endif /* DMG_H */
