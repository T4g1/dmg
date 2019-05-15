#ifndef CPU_GUI_H
#define CPU_GUI_H

#include <SDL2/SDL.h>

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
    void quit();

    Uint32 get_window_id();

private:
    bool running;

    CPU *cpu;

    SDL_Window *sdl_window;
    SDL_Surface *sdl_screen;
    SDL_PixelFormat *pixel_format;

    Uint32 last_refresh;

    void refresh_window();
};

#endif /* CPU_GUI_H */
