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
    input = new Input(&mmu);
    debugger = new Debugger(cpu, &mmu);

    running  = cart.load(path_rom);
    running &= ppu->init();
    running &= debugger->init();

    if (path_bios != nullptr) {
        running &= mmu.load(path_bios);
    }

    mmu.set_ppu(ppu);
    mmu.set_cartridge(&cart);

    cpu->reset();

    if (path_bios == nullptr) {
        no_boot();
    }

    return running;
}


/**
 * @brief      Main loop
 * @return     return code for the application
 */
int DMG::run()
{
    while (running) {
        if (!debugger->update()) {
            process();
        }

        debugger->draw();
        ppu->draw();

        handle_events();
    }

    debugger->quit();
    ppu->quit();

    return EXIT_SUCCESS;
}


/**
 * @brief      Dispatch process time to each DMG component
 */
void DMG::process()
{
    input->update();
    //timer->update();

    if (ppu->clock < cpu->clock) {
        ppu->step();
    } else {
        if (!cpu->step()) {
            error("CPU crash!\n");
            debugger->suspend_dmg = true;
        }
    }
}


/**
 * @brief      Dispatch events to all subsystems
 */
void DMG::handle_events()
{
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        debugger->handle_event(&event);
        input->handle(&event);

        if (event.type == SDL_QUIT) {
            running = false;
        } else if (event.type == SDL_WINDOWEVENT) {
            switch(event.window.event) {
            case SDL_WINDOWEVENT_CLOSE:
                Uint32 window_id = event.window.windowID;

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
}


/**
 * @brief      Cleanup
 */
void quit()
{
    SDL_Quit();
}


/**
 * @brief      Set the program to works without BOOT rom
 */
void DMG::no_boot()
{
    cpu->PC = 0x0100;

    mmu.set_boot_rom_enable(0x01);
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
