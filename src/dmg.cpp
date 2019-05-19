#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

#include "log.h"

#include "dmg.h"


bool DMG::init(const char *path_bios, const char *path_rom)
{
    // Setup SDL
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        error("Unable to initialize SDL\n");
        return false;
    }

    atexit(quit);

    cpu = new CPU(&mmu);
    ppu = new PPU(&mmu);

    mmu.set_ppu(ppu);

    cpu->reset();

    // ROM loading
    if (path_bios == nullptr) {
        cpu->PC = 0x0100;
        mmu.set_boot_rom_enable(0x01);
    } else if (!mmu.load(path_bios)) {
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

    debugger = new Debugger(cpu, &mmu);
    if (!debugger->init()) {
        return false;
    }

    running = true;

    return true;
}


/**
 * @brief      Main loop
 * @return     return code for the application
 */
int DMG::run()
{
    SDL_Event event;

    while (running) {
        // Process
        if (!debugger->update()) {
            if (ppu->clock < cpu->clock) {
                ppu->step();
            } else {
                if (!cpu->step()) {
                    error("CPU crash!\n");
                    running = false;
                    break;
                }
            }
        }

        // Display
        debugger->draw();
        ppu->draw();

        // Event
        while (SDL_PollEvent(&event)) {
            handle_event(&event);
        }
    }

    debugger->quit();
    ppu->quit();

    return EXIT_SUCCESS;
}


void DMG::handle_event(SDL_Event *event)
{
    debugger->handle_event(event);

    if (event->type == SDL_QUIT) {
        running = false;
    } else if (event->type == SDL_WINDOWEVENT) {
        switch(event->window.event) {
        case SDL_WINDOWEVENT_CLOSE:
            Uint32 window_id = event->window.windowID;

            if (window_id == debugger->get_window_id()) {
                debugger->close();
            }

            if (window_id == ppu->get_window_id()) {
                running = false;
            }
            break;
        }
    }
}


/**
 * @brief      Cleanup
 */
void quit()
{
    SDL_Quit();
}


/**
 * @brief      Change color palette to use (no param = default green one)
 * @param[in]  palette_index  0 default green
 *                            1 black/white
 */
void DMG::set_palette(char palette_index)
{
    if (palette_index < '0' || palette_index > '9') {
        return;
    }

    ppu->set_palette(palette_index - '0');
}
