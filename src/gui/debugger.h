#ifndef DEBUGGER_H
#define DEBUGGER_H

#include <GL/gl3w.h>
#include <SDL2/SDL.h>
#include <vector>
#include "imgui.h"
#include "imfilebrowser.h"

#include "../defines.h"

#define DEBUGGER_SAVE           ".breakpoints"

#define DEBUGGER_WIDTH          800
#define DEBUGGER_HEIGHT         600

#define TILESETS                3

// Draw an array of tiles with those dimensions
#define TILE_PER_ROW            8
#define TILE_PER_COLUMN         16

#define SPRITE_PER_ROW            5
#define SPRITE_PER_COLUMN         8

#define DISPLAY_TILE_WIDTH      TILE_WIDTH * 2
#define DISPLAY_TILE_HEIGHT     TILE_HEIGHT * 2

#define TOTAL_TILE_COUNT        TILESETS * TILE_PER_ROW * TILE_PER_COLUMN
#define RGB_TILE_SIZE           TILE_WIDTH * TILE_HEIGHT * 3


#define TRANSLATION(increment, fmt, ...) \
    snprintf(buffer, size, fmt, ##__VA_ARGS__); return increment;


struct Breakpoint {
    uint16_t address;
    bool read;      // Break on read
    bool write;     // Break on write
};


class CPU;
class MMU;
class DMG;
class PPU;
class APU;

/**
 * @brief      Displays emulator informations/status
 */
class Debugger {
public:
    // Which window to display
    bool show_execution;
    bool show_memory;
    bool show_vram;
    bool show_ppu;
    bool show_cpu;
    bool show_apu;
    bool show_breakpoints;

    bool show_demo; // DEBUG

    bool suspend_dmg;
    bool step_dmg;

    bool breakpoint_activated;
    std::vector<Breakpoint> breakpoints;


    Debugger();
    ~Debugger();

    bool init();
    bool update();
    void draw();
    void handle_event(SDL_Event *event);
    void show();
    void hide();
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
    void set_apu(APU *apu);

    void save();
    void load();
 ImGui::FileBrowser fileDialog;

private:
    bool running;
    int execution_speed;

    CPU *cpu;
    MMU *mmu;
    DMG *dmg;
    PPU *ppu;
    APU *apu;

    SDL_Window *sdl_window;
    SDL_PixelFormat *pixel_format;
    SDL_GLContext gl_context;

    Uint32 last_refresh;

    GLuint vram_tilemap[TOTAL_TILE_COUNT];
    GLuint oam_tilemap[OAM_COUNT];

    uint16_t translate(char buffer[], size_t size, uint16_t address);
    void ColorBoolean(bool condition);
    void ImageTileHoverable(ImTextureID texture);
    void ToggleButton(const char *text, bool *boolean);
    void BreakPointItem(Breakpoint *breakpoint);

    void display_memory();
    void display_registers();
    void display_execution();
    void display_PPU_status();
    void display_VRAM_status();
    void display_breakpoints();
    void display_load_game();
    void display_APU_status();
};

#endif /* DEBUGGER_H */
