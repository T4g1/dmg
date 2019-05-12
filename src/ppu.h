#ifndef PPU_H
#define PPU_H

#include <SDL2/SDL.h>

#include "mmu.h"

#define SCREEN_WIDTH            160
#define SCREEN_HEIGHT           144

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

/**
 * @brief      Pixel Processing Unit
 */
class PPU {
public:
    PPU(MMU *mmu);

    bool init();
    bool draw();
    void quit();

private:
    MMU *mmu;

    SDL_Window *sdl_window;
    SDL_Surface *sdl_screen;
    SDL_PixelFormat *pixel_format;

    Uint32 color_ldc_disabled;
    Uint32 palette[PALETTE_SIZE];

    uint16_t pixel_fifo;

    bool lcd_enabled;
    bool window_enabled;
    bool sprites_enabled;
    bool background_enabled;

    bool big_sprites;            // Sprites have height 8 if false, 16 if true

    uint16_t bg_window_tile_data_address;
    uint16_t bg_map_address;
    uint16_t window_map_address;

    void draw_background();
    void draw_window();
    void draw_sprites();
};

#endif /* PPU_H */
