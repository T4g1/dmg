#ifndef DEBUGGER_H
#define DEBUGGER_H

#include <GL/gl3w.h>
#include <SDL2/SDL.h>
#include "imgui.h"

#define DEBUGGER_WIDTH          800
#define DEBUGGER_HEIGHT         600

#define TILESETS                3

// Draw an array of tiles with those dimensions
#define TILE_PER_ROW            8
#define TILE_PER_COLUMN         16

#define DISPLAY_TILE_WIDTH      TILE_WIDTH * 2
#define DISPLAY_TILE_HEIGHT     TILE_HEIGHT * 2

#define TOTAL_TILE_COUNT        TILESETS * TILE_PER_ROW * TILE_PER_COLUMN
#define RGB_TILE_SIZE           TILE_WIDTH * TILE_HEIGHT * 3


#define TRANSLATION(increment, fmt, ...) \
    snprintf(buffer, size, fmt, ##__VA_ARGS__); return increment;

class CPU;
class MMU;
class DMG;
class PPU;

/**
 * @brief      Displays emulator informations/status
 */
class Debugger {
public:
    bool suspend_dmg;
    bool step_dmg;
    bool breakpoint_set;
    uint16_t breakpoint;

    Debugger();

    bool init();
    bool update();
    void draw();
    void handle_event(SDL_Event *event);
    void close();
    void quit();

    Uint32 get_window_id();

    size_t get_speed();
    void set_speed(size_t speed);

    // Accessor from outside
    void feed_memory_write(uint16_t address);
    void feed_memory_read(uint16_t address);

    void set_cpu(CPU *cpu);
    void set_mmu(MMU *mmu);
    void set_dmg(DMG *dmg);
    void set_ppu(PPU *ppu);

private:
    bool running;
    int execution_speed;

    CPU *cpu;
    MMU *mmu;
    DMG *dmg;
    PPU *ppu;

    SDL_Window *sdl_window;
    SDL_PixelFormat *pixel_format;
    SDL_GLContext gl_context;

    Uint32 last_refresh;

    char breakpoint_string[5];

    GLuint vram_tilemap[TOTAL_TILE_COUNT];

    uint16_t translate(char buffer[], size_t size, uint16_t address);
    void ColorBoolean(bool condition);

    void display_memory();
    void display_registers();
    void display_execution();
    void display_PPU_status();
    void display_VRAM_status();
};

#endif /* DEBUGGER_H */
