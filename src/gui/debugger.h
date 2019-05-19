#ifndef DEBUGGER_H
#define DEBUGGER_H

#include <SDL2/SDL.h>
#include "imgui.h"

#define DEBUGGER_WIDTH        800
#define DEBUGGER_HEIGHT       600

#define TRANSLATION(increment, fmt, ...) \
    snprintf(buffer, size, fmt, ##__VA_ARGS__); return address + increment;

class CPU;
class MMU;

/**
 * @brief      Displays emulator informations/status
 */
class Debugger {
public:
    Debugger(CPU *cpu, MMU *mmu);

    bool init();
    bool update();
    void draw();
    void handle_event(SDL_Event *event);
    void close();
    void quit();

    Uint32 get_window_id();

private:
    bool running;

    bool suspend_dmg;

    CPU *cpu;
    MMU *mmu;

    SDL_Window *sdl_window;
    SDL_Surface *sdl_screen;
    SDL_PixelFormat *pixel_format;
    SDL_GLContext gl_context;

    Uint32 last_refresh;

    uint16_t translate(char buffer[], size_t size, uint16_t address);

    void display_memory();
    void display_registers();
    void display_execution();
};

#endif /* DEBUGGER_H */
