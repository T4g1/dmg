#ifndef PPU_H
#define PPU_H

#include <SDL2/SDL.h>

#include "mmu.h"

#define LINE_Y_COUNT        144
#define LINE_X_COUNT        160

#define SCREEN_WIDTH            LINE_X_COUNT
#define SCREEN_HEIGHT           LINE_Y_COUNT


#define BIT_LCD_ENABLED                     7
#define BIT_WINDOW_MAP_SELECT               6
#define BIT_WINDOW_ENABLED                  5
#define BIT_BG_WINDOW_TILE_DATA_SELECT      4
#define BIT_BG_MAP_SELECT                   3
#define BIT_SPRITES_SIZE                    2
#define BIT_SPRITES_ENABLED                 1
#define BIT_BACKGROUND_ENABLED              0

#define PALETTE_SIZE            4       // How many colors available

#define V_BLANK_PERIOD      10
#define MAX_LY              LINE_Y_COUNT + V_BLANK_PERIOD

// Clock timing
#define CLOCK_OAM_SEARCH        20
#define CLOCK_PIXEL_TRANSFER    43
#define CLOCK_H_BLANK           51
#define CLOCK_V_BLANK           CLOCK_OAM_SEARCH + CLOCK_PIXEL_TRANSFER + CLOCK_H_BLANK

#define BG_MAP_WIDTH            32
#define BG_MAP_HEIGHT           32

#define PIXEL_SIZE              2      // Size of a pixel data in bit

// TODO: SPRITE_HEIGHT should be handled by the BIT_SPRITE_SIZE in LCDC
#define TILE_HEIGHT             8
#define TILE_WIDTH              8
#define TILE_LINE_SIZE          2       // Size of a line of a tile in bytes

// Size of a tile in memory in bytes. Should be 0x10
#define TILE_SIZE               TILE_HEIGHT * TILE_LINE_SIZE

#define FIFO_SIZE               16

/**
 * @brief      Pixel Processing Unit
 */
class PPU {
public:
    PPU(MMU *mmu);

    bool init();
    bool draw();
    void quit();

    size_t clock;

    void set_lcdc(uint8_t lcdc);
    void set_bgp(uint8_t bgp);

    Uint32 get_window_id();

private:
    MMU *mmu;

    SDL_Window *sdl_window;
    SDL_Surface *sdl_screen;
    SDL_PixelFormat *pixel_format;

    Uint32 color_ldc_disabled;
    Uint32 palette[PALETTE_SIZE];
    Uint32 bg_palette[PALETTE_SIZE];

    bool lcd_enabled;
    bool window_enabled;
    bool sprites_enabled;
    bool background_enabled;

    //size_t sprite_height;

    uint16_t bg_window_tile_data_address;
    uint16_t bg_map_address;
    uint16_t window_map_address;

    uint8_t pixel_fifo[FIFO_SIZE];
    size_t pf_size;             // How many pixels in the FIFO
    size_t pf_index;            // Position in the FIFO

    bool draw_line();
    void fetch(uint8_t scx, uint8_t scy, size_t x, size_t ly);
};

#endif /* PPU_H */
