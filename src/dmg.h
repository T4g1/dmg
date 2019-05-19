#ifndef DMG_H
#define DMG_H

#include "ppu.h"
#include "cpu.h"
#include "input.h"
#include "cartridge.h"
#include "gui/debugger.h"

#define FPS                 30
#define GLSL_VERSION        "#version 130"
#define FRAME_CYCLES        70244  //<! Amount of CPU cycle between each frame


void quit();


/**
 * @brief      DMG Emulator
 */
class DMG {
    Cartridge cart;
    MMU mmu;
    CPU *cpu;
    PPU *ppu;
    Input *input;

    Debugger *debugger;

    bool running;

public:
    bool init(const char *path_bios, const char *path_rom);
    int run();
    void handle_event(SDL_Event *event);

    void set_palette(char palette_index);
};

#endif /* DMG_H */
