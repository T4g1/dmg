#include "cpu_gui.h"

#include "../log.h"
#include "../cpu.h"


CPUGui::CPUGui(CPU *cpu) : cpu(cpu)
{
    running = false;
}


bool CPUGui::init()
{
    sdl_window = SDL_CreateWindow(
        "DMG - CPU GUI",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        CPUGUI_WIDTH,
        CPUGUI_HEIGHT,
        SDL_WINDOW_SHOWN
    );
    if(sdl_window == NULL) {
        error("CPU GUI: nable to create a display window\n");
        return false;
    }

    sdl_screen = SDL_GetWindowSurface(sdl_window);
    if (sdl_screen == nullptr) {
        error("Unable to get the screen surface\n");
        return false;
    }

    running = true;

    pixel_format = sdl_screen->format;

    last_refresh = 0;

    return true;
}


void CPUGui::update()
{
    if (!running) {
        return;
    }

    Uint32 current_ticks = SDL_GetTicks();
    if (current_ticks < last_refresh + 1000) {
        return;
    }

    refresh_window();

    last_refresh = current_ticks;
}


void CPUGui::refresh_window()
{
    SDL_FillRect(sdl_screen, NULL, SDL_MapRGB(pixel_format, 150, 125, 16));
    SDL_UpdateWindowSurface(sdl_window);
}


void CPUGui::quit()
{
    running = false;

    SDL_DestroyWindow(sdl_window);
}


Uint32 CPUGui::get_window_id()
{
    return SDL_GetWindowID(sdl_window);
}
