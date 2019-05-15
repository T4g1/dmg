#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <SDL2/SDL.h>

#include "log.h"

#include "dmg.h"
#include <unistd.h>     // DEBUG


bool DMG::init(const char *path_bios, const char *path_rom)
{
    running = false;

    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        error("Unable to initialize SDL\n");
        return false;
    }

    atexit(SDL_Quit);

    cpu = new CPU(&mmu);
    ppu = new PPU(&mmu);

    mmu.set_ppu(ppu);

    cpu->reset();

    // ROM loading
    if (!mmu.load(path_bios)) {
        return false;
    }

    if (!cart.load(path_rom)) {
        return false;
    }

    mmu.set_cartridge(&cart);

    // Graphic initialization
    if (!ppu->init()) {
        return false;
    }

#ifdef DEBUG
    cpu_gui = new CPUGui(cpu);
    if (!cpu_gui->init()) {
        return false;
    }
#endif

    running = true;

    return true;
}


int DMG::run()
{
    SDL_Event event;

    // Initial black screen
    ppu->draw();

    while (running) {
        if (ppu->clock < cpu->clock) {
            ppu->draw();
        } else {
            if (!cpu->step()) {
                error("CPU crash!\n");
                running = false;
                break;
            }
        }

#ifdef DEBUG
        cpu_gui->update();
#endif

        while (SDL_PollEvent(&event)) {
            handle_event(&event);
        }
    }

    ppu->quit();

#ifdef DEBUG
    cpu_gui->quit();
#endif

    return EXIT_SUCCESS;
}


void DMG::handle_event(SDL_Event *event)
{
    if (event->type == SDL_QUIT) {
        running = false;
    } else if (event->type == SDL_WINDOWEVENT) {
        switch(event->window.event) {
        case SDL_WINDOWEVENT_CLOSE:
            Uint32 window_id = event->window.windowID;

#ifdef DEBUG
            if (window_id == cpu_gui->get_window_id()) {
                cpu_gui->quit();
            }
#endif
            if (window_id == ppu->get_window_id()) {
                running = false;
            }
            break;
        }
    }
}
