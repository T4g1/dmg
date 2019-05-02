#ifndef DMG_H
#define DMG_H

#include "cpu.h"
#include "mmu.h"
#include "cartridge.h"

#define FRAME_CYCLES        70244  //<! Amount of CPU cycle between each frame


/**
 * @brief      DMG Emulator
 */
class DMG {
    Cartridge cart;
    MMU mmu;
    CPU *cpu;
    //GPU *gpu;

public:
    int run(const char *path_bios, const char *path_rom);
    bool step_frame();
};

#endif /* DMG_H */
