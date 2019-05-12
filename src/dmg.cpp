#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <SDL2/SDL.h>

#include "log.h"

#include "dmg.h"
#include <unistd.h>     // DEBUG


int DMG::run(const char *path_bios, const char *path_rom)
{
    SDL_Event event;

    cpu = new CPU(&mmu);
    ppu = new PPU(&mmu);

    cpu->reset();

    // ROM loading
    if (!mmu.load(path_bios)) {
        return EXIT_FAILURE;
    }

    if (!cart.load(path_rom)) {
        return EXIT_FAILURE;
    }

    mmu.set_cartridge(&cart);

    // Graphic initialization
    if (!ppu->init()) {
        return EXIT_FAILURE;
    }

    // Initial black screen
    ppu->draw();

    bool running = true;
    while (running) {
        //info("FRAME: %zu CPU: %zu PC: 0x%04X PPU: %zu\n", ppu->frame, cpu->clock, cpu->PC, ppu->clock);

        if (ppu->clock < cpu->clock) {
            ppu->draw();
        } else {
            cpu->step();
        }

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }
    }

    ppu->quit();

    return EXIT_SUCCESS;
}


/**
 * @brief      Give time to CPU while no frame should be draw
 * @return     true when a frame should be rendered
 */
bool DMG::step_frame()
{
    while (cpu->clock < FRAME_CYCLES) {
        cpu->step();
    }

    cpu->clock -= FRAME_CYCLES;

    return true;
}
