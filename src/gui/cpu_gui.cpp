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

    return true;
}


void CPUGui::update()
{
    if (!running) {
        return;
    }

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
