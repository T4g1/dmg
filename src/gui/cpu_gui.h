#ifndef CPU_GUI_H
#define CPU_GUI_H

#include <SDL2/SDL.h>
#include "imgui.h"

#define CPUGUI_WIDTH        800
#define CPUGUI_HEIGHT       600

class CPU;

/**
 * @brief      Displays what the CPU is up to
 */
class CPUGui {
public:
    CPUGui(CPU *cpu);

    bool init();
    void update();
    void handle_event(SDL_Event *event);
    void close();

    Uint32 get_window_id();

private:
    bool running;

    CPU *cpu;

    SDL_Window *sdl_window;
    SDL_Surface *sdl_screen;
    SDL_PixelFormat *pixel_format;
    SDL_GLContext gl_context;

    Uint32 last_refresh;

    void refresh_window();
};

#endif /* CPU_GUI_H */
