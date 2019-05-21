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

    mmu = new MMU();
    cpu = new CPU();
    ppu = new PPU();
    timer = new Timer();
    input = new Input();
    debugger = new Debugger();

    mmu->set_ppu(ppu);
    mmu->set_timer(timer);
    mmu->set_input(input);
    cpu->set_mmu(mmu);
    ppu->set_mmu(mmu);
    timer->set_mmu(mmu);
    input->set_mmu(mmu);
    debugger->set_cpu(cpu);
    debugger->set_mmu(mmu);
    debugger->set_ppu(ppu);
    debugger->set_dmg(this);

    running  = cart.load(path_rom);
    running &= mmu->init(path_bios, &cart);
    running &= cpu->init();
    running &= ppu->init();
    running &= timer->init();
    running &= input->init();
    running &= debugger->init();

    if (path_bios == nullptr) {
        no_boot();
    }

    system_clock = 0;
    set_speed(DEFAULT_SPEED);

    return running;
}


/**
 * @brief      Main loop
 * @return     return code for the application
 */
int DMG::run()
{
    while (running) {
        for (size_t i=0; i<debugger->get_speed(); i++) {
            if (!debugger->update()) {
                process();
            }
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

    if (system_clock >= cpu->clock) {
        if (cpu->step()) {
            debugger->step_dmg = false;
        }
    }

    if (system_clock >= ppu->clock) {
        ppu->step();
    }

    if (system_clock >= timer->clock) {
        timer->step();
    }

    // Update system clock
    system_clock = timer->clock;
    if (ppu->clock < system_clock) {
        system_clock = ppu->clock;
    }
    if (cpu->clock < system_clock) {
        system_clock = cpu->clock;
    }

    // TODO: Prevent clock overflow by substracting some value from them all
}


/**
 * @brief      Dispatch events to all subsystems
 */
void DMG::handle_events()
{
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        debugger->handle_event(&event);

        if (event.window.windowID == ppu->get_window_id()) {
            input->handle(&event);
        }

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


void DMG::reset()
{
    mmu->reset();
    cpu->reset();
    ppu->reset();
    timer->reset();
    input->reset();
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

    mmu->set_booted(true);
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


void DMG::set_speed(size_t speed)
{
    debugger->set_speed(speed);
}
