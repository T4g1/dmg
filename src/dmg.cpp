#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <SDL/SDL.h>

#include "log.h"

#include "dmg.h"


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

    bool running = true;
    while (running) {
        step_frame();

        ppu->draw();

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }
    }

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
