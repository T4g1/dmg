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


DMG::~DMG()
{
    delete debugger;
    delete apu;
    delete input;
    delete timer;
    delete ppu;
    delete cpu;
    delete mmu;

    debugger = nullptr;
    apu = nullptr;
    input = nullptr;
    timer = nullptr;
    ppu = nullptr;
    cpu = nullptr;
    mmu = nullptr;

    SDL_Quit();
}


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

    mmu = new MMU();
    cpu = new CPU();
    ppu = new PPU();
    timer = new Timer();
    input = new Input();
    apu = new APU();
    debugger = new Debugger();

    mmu->set_ppu(ppu);
    mmu->set_timer(timer);
    mmu->set_input(input);
    mmu->set_apu(apu);
    mmu->set_debugger(debugger);
    cpu->set_mmu(mmu);
    ppu->set_mmu(mmu);
    timer->set_mmu(mmu);
    input->set_mmu(mmu);
    apu->set_mmu(mmu);
    debugger->set_cpu(cpu);
    debugger->set_mmu(mmu);
    debugger->set_ppu(ppu);
    debugger->set_dmg(this);

    running  = true;
    running &= mmu->init(this->bios_path, this->rom_path);
    running &= cpu->init();
    running &= ppu->init();
    running &= timer->init();
    running &= input->init();
    running &= apu->init();
    running &= debugger->init();

    set_palette(palette);
    set_speed(DEFAULT_SPEED);

    reset();

    save_slot = 0;
    dmg_clock = 0;
    system_clock = 0;
    last_tick = 0;

    return running;
}


/**
 * @brief      Main loop
 * @return     return code for the application
 */
int DMG::run()
{
    while (running) {
        Uint32 current_tick = SDL_GetTicks();
        dmg_clock += 4195 * (current_tick - last_tick); //  4194304 / 1000
        last_tick = current_tick;

        for (size_t i=0; i<debugger->get_speed(); i++) {
            if (!debugger->update() && system_clock < dmg_clock) {
                process();
            }
        }

        debugger->draw();

        handle_events();
    }

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

    if (system_clock >= apu->clock) {
        apu->step();
    }
}


/**
 * @brief      Set system clock to the lowest clock
 */
void DMG::update_system_clock()
{
    system_clock = apu->clock;
    if (ppu->clock < system_clock) {
        system_clock = ppu->clock;
    }
    if (cpu->clock < system_clock) {
        system_clock = cpu->clock;
    }
    if (timer->clock < system_clock) {
        system_clock = timer->clock;
    }

    // TODO: Use class methods to do this so internal clock are shifted too
    // last_increment and last_div_increment for timer for example
    //
    /*
    if (system_clock > 0x100000000) {
        timer->clock -= 0x80000000;
        cpu->clock -= 0x80000000;
        ppu->clock -= 0x80000000;
        apu->clock -= 0x80000000;
    }
    */
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
            case SDLK_F4:   // Next slot
                select_next_save_slot();
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
    mmu->reset();
    cpu->reset();
    ppu->reset();
    timer->reset();
    input->reset();
    apu->reset();

    // No boot rom, skip to game start
    if (mmu->no_boot) {
        cpu->PC = 0x0100;
    }
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
    mmu->load_rom(rom_path);

    reset();
}


std::string DMG::get_save_name()
{
    return rom_path + ".state" + std::to_string(save_slot) + ".sav";
}


/**
 * @brief      Selects previous save slots
 */
void DMG::select_prev_save_slot()
{
    if (save_slot == 0) {
        save_slot = SAVE_SLOT_COUNT;
    }

    save_slot -= 1;
}


/**
 * @brief      Selects next save slots
 */
void DMG::select_next_save_slot()
{
    save_slot += 1;

    if (save_slot >= SAVE_SLOT_COUNT) {
        save_slot = 0;
    }
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
    apu->serialize(file);

    file.close();
}


/**
 * @brief      Loads game state
 */
void DMG::load_state()
{
    std::ifstream file;
    file.open(get_save_name(), std::ios::binary);
    if (!file.is_open()) {
        return;
    }

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
        apu->deserialize(file);
    }

    file.close();
}
