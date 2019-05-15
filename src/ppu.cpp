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

    clock = 0;
}

bool PPU::init()
{
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

    // Fixed colors of the DMG
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
        if (draw_line()) {
            return SDL_UpdateWindowSurface(sdl_window) == 0;
        }
    } else {
        SDL_FillRect(sdl_screen, NULL, color_ldc_disabled);

        // TODO: Handle LCD control register so PPU gets CPU clock time when LCD is on
        // DEBUG: Arbitrary increase wiating for display to be active
        clock += CLOCK_H_BLANK;

        return SDL_UpdateWindowSurface(sdl_window) == 0;
    }

    return true;
}

/**
 * @brief      Draws a line.
 * @return     true if the screen can be refreshed
 */
bool PPU::draw_line()
{
    pf_size = 0;
    pf_index = 0;

    uint8_t ly = mmu->get(LY);

    if (ly < LINE_Y_COUNT) {
        // Viewport position
        uint8_t scy = mmu->get(SCY);
        uint8_t scx = mmu->get(SCX);

        fetch(scx, scy, 0, ly);

        for (size_t x=0; x<LINE_X_COUNT; x++) {
            // Fetch next 8 pixels
            if (pf_size <= TILE_WIDTH) {
                fetch(scx, scy, x, ly);
            }

            size_t pixel = pixel_fifo[pf_index];
            pf_index = (pf_index + 1) % FIFO_SIZE;
            pf_size -= 1;

            // TODO: Check if pixel is BG, Window or Sprite
            set_pixel(sdl_screen, x, ly, bg_palette[pixel]);
        }

        clock += CLOCK_OAM_SEARCH;
        clock += CLOCK_PIXEL_TRANSFER;

        // H-Blank
        clock += CLOCK_H_BLANK;

        ly += 1;
        mmu->set(LY, ly);
    }
    // V-Blank
    else if (ly < MAX_LY) {
        clock += CLOCK_V_BLANK;

        ly += 1;
        mmu->set(LY, ly);
    }
    // Reset LY
    else {
        mmu->set(LY, 0);
    }

    // Arbitrary choice to refresh SDL screen at last line draw
    return ly == LINE_Y_COUNT;
}

void PPU::fetch(uint8_t scx, uint8_t scy, size_t x, size_t ly)
{
    // We want the position of the viewport)
    // We add pf_size so we get the pixels needed when the fifo arrives at those pixels
    size_t viewport_x = (scx + x + pf_size) % (BG_MAP_WIDTH * TILE_WIDTH);
    size_t viewport_y = (scy + ly) % (BG_MAP_HEIGHT * TILE_HEIGHT);

    // Index of the tile in the BG map
    size_t tile_x = viewport_x / TILE_WIDTH;
    size_t tile_y = viewport_y / TILE_HEIGHT;

    // Address of the sprite in the BG map
    uint16_t map_address = bg_map_address + (tile_y * BG_MAP_WIDTH) + tile_x;

    // Tile index in the BG tileset
    uint8_t tile_id  = mmu->get(map_address);

    // Address of the sprite in the BG tileset
    uint16_t tile_address = bg_window_tile_data_address + (tile_id * TILE_SIZE);

    // Data for the current line of the tile being drawn
    uint16_t tile_line_address = tile_address + TILE_LINE_SIZE * (viewport_y % TILE_HEIGHT);

    uint8_t data1 = mmu->get(tile_line_address);
    uint8_t data2 = mmu->get(tile_line_address + 1);

    // Exctract the 8 pixels for the data
    for (size_t i=0; i<TILE_WIDTH; i++) {
        // 8 first bit (data1) = first bit of data for the pixels
        // 8 last bit (data2) = second bit of data for the pixels
        // b7 b6 b5 b4 b3 b2 b1 b0 so 7 - i to get correct bit
        size_t bit1 = get_bit(data1, 7 - i);
        size_t bit2 = get_bit(data2, 7 - i);

        pixel_fifo[(pf_index + pf_size++) % FIFO_SIZE] = (bit1 << 1) + bit2;
    }
}

void PPU::quit()
{
    SDL_FreeSurface(sdl_screen);
    SDL_DestroyWindow(sdl_window);
}


void PPU::set_lcdc(uint8_t lcdc)
{
    lcd_enabled = get_bit(lcdc, BIT_LCD_ENABLED);

    window_enabled = get_bit(lcdc, BIT_WINDOW_ENABLED);
    sprites_enabled = get_bit(lcdc, BIT_SPRITES_ENABLED);
    background_enabled = get_bit(lcdc, BIT_BACKGROUND_ENABLED);

    // Where are the tiles for gackground and window
    bg_window_tile_data_address = TILE_ADDRESS_2;
    if (get_bit(lcdc, BIT_BG_WINDOW_TILE_DATA_SELECT)) {
        bg_window_tile_data_address = TILE_ADDRESS_1;
    }

    // Where are the data for background
    bg_map_address = MAP_ADDRESS_1;
    if (get_bit(lcdc, BIT_BG_MAP_SELECT)) {
        bg_map_address = MAP_ADDRESS_2;
    }

    // Where are the data for window
    window_map_address = MAP_ADDRESS_1;
    if (get_bit(lcdc, BIT_WINDOW_MAP_SELECT)) {
        window_map_address = MAP_ADDRESS_2;
    }

    /*sprite_height = 8;
    if (get_bit(lcdc, BIT_SPRITES_SIZE)) {
        sprite_height = 16;
    }*/
}


/**
 * @brief      Set the palette used for background tiles
 * @param[in]  bgp  BGP value
 */
void PPU::set_bgp(uint8_t bgp)
{
    bg_palette[0b00] = palette[(bgp & 0b00000011)     ];
    bg_palette[0b01] = palette[(bgp & 0b00001100) >> 2];
    bg_palette[0b10] = palette[(bgp & 0b00110000) >> 4];
    bg_palette[0b11] = palette[(bgp & 0b11000000) >> 6];
}


Uint32 PPU::get_window_id()
{
    return SDL_GetWindowID(sdl_window);
}
