#include "ppu.h"

#include <SDL2/SDL.h>

#include "log.h"
#include "utils.h"

PPU::PPU(MMU *mmu) : mmu(mmu)
{
    lcd_enabled = false;
    window_enabled = false;
    sprites_enabled = false;
    background_enabled = false;

    pixel_fifo = 0x00;

    mmu->call_register(ADDR_LCDC, &on_lcd_control_set);
}

bool PPU::init()
{
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        error("Unable to initialize SDL\n");
        return false;
    }

    atexit(SDL_Quit);

    sdl_window = SDL_CreateWindow(
        "DMG - Emulator",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        SCREEN_WIDTH,
        SCREEN_HEIGHT,
        SDL_WINDOW_SHOWN
    );
    if(sdl_window == NULL) {
        error("Unable to create a display window\n");
        return false;
    }

    sdl_screen = SDL_GetWindowSurface(sdl_window);
    if (sdl_screen == nullptr) {
        error("Unable to get the screen surface\n");
        return false;
    }

    pixel_format = sdl_screen->format;

    color_ldc_disabled = SDL_MapRGB(pixel_format, 150, 125, 16);

    palette[0b00] = SDL_MapRGB(pixel_format, 110, 125, 70);
    palette[0b01] = SDL_MapRGB(pixel_format,  80, 105, 75);
    palette[0b10] = SDL_MapRGB(pixel_format,  60,  90, 85);
    palette[0b11] = SDL_MapRGB(pixel_format,  60,  80, 75);

    return true;
}

/**
 * @brief      Refresh the screen
 */
bool PPU::draw()
{
    if (lcd_enabled) {
        SDL_FillRect(sdl_screen, NULL, palette[0b00]);
        if (background_enabled) {
            draw_background();
        }

        if (window_enabled) {
            draw_window();
        }

        if (sprites_enabled) {
            draw_sprites();
        }
    } else {
        SDL_FillRect(sdl_screen, NULL, color_ldc_disabled);
    }

    return SDL_UpdateWindowSurface(sdl_window) == 0;
}

void PPU::on_lcd_control_set()
{
    uint8_t lcdc = mmu->get(ADDR_LCDC);
    lcd_enabled = get_bit(lcdc, BIT_LCD_ENABLED);
    window_enabled = get_bit(lcdc, BIT_WINDOW_ENABLED);
    sprites_enabled = get_bit(lcdc, BIT_SPRITES_ENABLED);
    background_enabled = get_bit(lcdc, BIT_BACKGROUND_ENABLED);
}

void PPU::draw_background()
{
}

void PPU::draw_window()
{
}

void PPU::draw_sprites()
{
}

void PPU::quit()
{
}
