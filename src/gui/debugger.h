#ifndef DEBUGGER_H
#define DEBUGGER_H

#include <SDL2/SDL.h>
#include "imgui.h"

#define DEBUGGER_WIDTH        800
#define DEBUGGER_HEIGHT       600

class CPU;
class MMU;

/**
 * @brief      Displays emulator informations/status
 */
class Debugger {
public:
    Debugger(CPU *cpu, MMU *mmu);

    bool init();
    void update();
    void handle_event(SDL_Event *event);
    void close();
    void quit();

    Uint32 get_window_id();

private:
    bool running;

    CPU *cpu;
    MMU *mmu;

    SDL_Window *sdl_window;
    SDL_Surface *sdl_screen;
    SDL_PixelFormat *pixel_format;
    SDL_GLContext gl_context;

    Uint32 last_refresh;

    void refresh_window();
    void display_memory();
};

#endif /* DEBUGGER_H */
