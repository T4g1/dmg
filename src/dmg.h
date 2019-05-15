#ifndef DMG_H
#define DMG_H

#include "ppu.h"
#include "cpu.h"
#include "cartridge.h"
#include "gui/cpu_gui.h"

#define FRAME_CYCLES        70244  //<! Amount of CPU cycle between each frame


/**
 * @brief      DMG Emulator
 */
class DMG {
    Cartridge cart;
    MMU mmu;
    CPU *cpu;
    PPU *ppu;

    CPUGui *cpu_gui;

    bool running;

public:
    bool init(const char *path_bios, const char *path_rom);
    int run();
    void handle_event(SDL_Event *event);
    void set_palette(char palette_index);
};

#endif /* DMG_H */
