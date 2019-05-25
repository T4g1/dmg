#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <fstream>
#include "imgui.h"
#include "imgui_impl_sdl.h"
#include "imgui_impl_opengl3.h"

#include "log.h"

#include "dmg.h"


bool DMG::init(const char *bios_path, const char *rom_path)
{
    this->bios_path = "";
    if (bios_path != nullptr) {
        this->bios_path = bios_path;
    }

    this->rom_path = "";
    if (rom_path != nullptr) {
        this->rom_path = rom_path;
    }

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
    mmu->set_debugger(debugger);
    cpu->set_mmu(mmu);
    ppu->set_mmu(mmu);
    timer->set_mmu(mmu);
    input->set_mmu(mmu);
    debugger->set_cpu(cpu);
    debugger->set_mmu(mmu);
    debugger->set_ppu(ppu);
    debugger->set_dmg(this);

    running = debugger->init();

    set_speed(DEFAULT_SPEED);

    load_rom(rom_path);

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
    update_system_clock();

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

    update_system_clock();
}


/**
 * @brief      Set system clock to the lowest clock
 */
void DMG::update_system_clock()
{
    system_clock = timer->clock;
    if (ppu->clock < system_clock) {
        system_clock = ppu->clock;
    }
    if (cpu->clock < system_clock) {
        system_clock = cpu->clock;
    }

    // FIXME: THis may not be necessary given size limit of size_t
    // TODO: Check i does not affect timing
    if (system_clock > 0x100000000) {
        timer->clock -= 0x80000000;
        cpu->clock -= 0x80000000;
        ppu->clock -= 0x80000000;
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

        if (event.window.windowID == ppu->get_window_id()) {
            input->handle(&event);
        }

        switch(event.type) {
        case SDL_QUIT:
            running = false;
            break;
        case SDL_WINDOWEVENT:
            switch(event.window.event) {
            case SDL_WINDOWEVENT_CLOSE:
                Uint32 window_id = event.window.windowID;

                if (window_id == debugger->get_window_id()) {
                    debugger->hide();
                }

                if (window_id == ppu->get_window_id()) {
                    running = false;
                }
                break;
            }
            break;
        case SDL_KEYDOWN:
            switch(event.key.keysym.sym){
            case SDLK_F3:   // Save state
                save_state();
                break;
            case SDLK_F5:   // Load state
                load_state();
                break;
            }
            break;
        }
    }
}


void DMG::reset()
{
    load_rom(rom_path);
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
void DMG::fake_boot()
{
    cpu->PC = 0x0100;

    mmu->set_booted(true);
}


/**
 * @brief      Change color palette to use (no param = default green one)
 * @param[in]  palette_index  0 default green
 *                            1 black/white
 */
void DMG::set_palette(size_t palette_index)
{
    palette = palette_index;

    ppu->set_palette(palette);
}


void DMG::set_speed(size_t speed)
{
    debugger->set_speed(speed);
}


/**
 * @brief      Loads the given ROM
 * @param[in]  filepath  The filepath
 */
void DMG::load_rom(std::string filepath)
{
    rom_path = filepath;

    // Bad cartridge are a correct use case
    cart.load(rom_path);

    running &= mmu->init(bios_path.c_str(), &cart);
    running &= cpu->init();
    running &= ppu->init();
    running &= timer->init();
    running &= input->init();

    set_palette(palette);

    if (bios_path.size() <= 0) {
        fake_boot();
    }
}


std::string DMG::get_save_name()
{
    return rom_path + ".state0.sav";
}


/**
 * @brief      Save game state
 */
void DMG::save_state()
{
    std::ofstream file;
    file.open(get_save_name(), std::ios::binary);

    cpu->serialize(file);
    mmu->serialize(file);
    ppu->serialize(file);
    timer->serialize(file);
    input->serialize(file);

    file.close();
}


/**
 * @brief      Loads game state
 */
void DMG::load_state()
{
    std::ifstream file;
    file.open(get_save_name(), std::ios::binary);

    while (!file.eof()) {
        int c = file.peek();
        if (c == EOF) {
            break;
        }

        cpu->deserialize(file);
        mmu->deserialize(file);
        ppu->deserialize(file);
        timer->deserialize(file);
        input->deserialize(file);
    }

    file.close();
}
