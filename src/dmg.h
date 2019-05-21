#ifndef DMG_H
#define DMG_H

#include "ppu.h"
#include "cpu.h"
#include "input.h"
#include "timer.h"
#include "cartridge.h"
#include "gui/debugger.h"

#define FPS                 30
#define GLSL_VERSION        "#version 130"
#define FRAME_CYCLES        70244       //<! Amount of CPU cycle between each frame
#define DEFAULT_SPEED       10          // Speed of the emulator


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

    size_t system_clock;

public:
    bool init(const char *path_bios, const char *path_rom);
    int run();
    void process();
    void handle_events();
    void reset();

    void no_boot();
    void set_palette(char palette_index);
    void set_speed(size_t speed);
};

#endif /* DMG_H */
