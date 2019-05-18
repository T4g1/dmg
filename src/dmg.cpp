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

    cpu_gui = new CPUGui(cpu);
    if (!cpu_gui->init()) {
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

        cpu_gui->update();

        while (SDL_PollEvent(&event)) {
            handle_event(&event);
        }
    }

    cpu_gui->close();
    ppu->quit();

    return EXIT_SUCCESS;
}


void DMG::handle_event(SDL_Event *event)
{
    cpu_gui->handle_event(event);

    if (event->type == SDL_QUIT) {
        running = false;
    } else if (event->type == SDL_WINDOWEVENT) {
        switch(event->window.event) {
        case SDL_WINDOWEVENT_CLOSE:
            Uint32 window_id = event->window.windowID;

            if (window_id == cpu_gui->get_window_id()) {
                cpu_gui->close();
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
