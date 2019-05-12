#ifndef PPU_H
#define PPU_H

#include <SDL2/SDL.h>

#include "mmu.h"

#define SCREEN_WIDTH            160
#define SCREEN_HEIGHT           144

#define ADDR_LCDC               0xFF40  // LCD Controle register

#define BIT_LCD_ENABLED         7
#define BIT_WINDOW_ENABLED      5
#define BIT_SPRITES_ENABLED     1
#define BIT_BACKGROUND_ENABLED  0

#define PALETTE_SIZE            4       // How many colors available

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

    void draw_background();
    void draw_window();
    void draw_sprites();

    void on_lcd_control_set();
};

#endif /* PPU_H */
