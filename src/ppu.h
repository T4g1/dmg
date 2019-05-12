#ifndef PPU_H
#define PPU_H

#include <SDL2/SDL.h>

#include "mmu.h"

#define LINE_Y_COUNT        144
#define LINE_X_COUNT        160

#define SCREEN_WIDTH            LINE_X_COUNT
#define SCREEN_HEIGHT           LINE_Y_COUNT

#define ADDR_LCDC               0xFF40  // LCD Controle register

#define BIT_LCD_ENABLED                     7
#define BIT_WINDOW_MAP_SELECT               6
#define BIT_WINDOW_ENABLED                  5
#define BIT_BG_WINDOW_TILE_DATA_SELECT      4
#define BIT_BG_MAP_SELECT                   3
#define BIT_SPRITES_SIZE                    2
#define BIT_SPRITES_ENABLED                 1
#define BIT_BACKGROUND_ENABLED              0

#define PALETTE_SIZE            4       // How many colors available

#define TILE_ADDRESS_1      0x8000
#define TILE_ADDRESS_2      0x8800
#define MAP_ADDRESS_1       0x9800
#define MAP_ADDRESS_2       0x9C00

#define BG_SCY              0xFF42
#define BG_SCX              0xFF43
#define LY                  0xFF44
#define LYC                 0xFF45      // TODO

#define V_BLANK_PERIOD      10
#define MAX_LY              LINE_Y_COUNT + V_BLANK_PERIOD

// Clock timing
#define CLOCK_OAM_SEARCH        20
#define CLOCK_PIXEL_TRANSFER    43
#define CLOCK_H_BLANK           51
#define CLOCK_V_BLANK           CLOCK_OAM_SEARCH + CLOCK_PIXEL_TRANSFER + CLOCK_H_BLANK

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

    //DEBUG
    size_t frame;

private:
    MMU *mmu;

    SDL_Window *sdl_window;
    SDL_Surface *sdl_screen;
    SDL_PixelFormat *pixel_format;

    Uint32 color_ldc_disabled;
    Uint32 palette[PALETTE_SIZE];

    bool lcd_enabled;
    bool window_enabled;
    bool sprites_enabled;
    bool background_enabled;

    bool big_sprites;            // Sprites have height 8 if false, 16 if true

    uint16_t bg_window_tile_data_address;
    uint16_t bg_map_address;
    uint16_t window_map_address;

    bool draw_line();
};

#endif /* PPU_H */
