#include "ppu.h"

#include <SDL2/SDL.h>

#include "dmg.h"
#include "log.h"
#include "utils.h"

PPU::PPU(MMU *mmu) : mmu(mmu)
{
    lcd_enabled = false;
    window_enabled = false;
    sprites_enabled = false;
    background_enabled = false;

    clock = 0;

    last_refresh = 0;
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

    set_palette(0);

    return true;
}


/**
 * @brief      Emulates the PPU
 */
void PPU::step()
{
    if (lcd_enabled) {
        draw_line();
    } else {
        SDL_FillRect(sdl_screen, NULL, color_ldc_disabled);

        // TODO: Handle LCD control register so PPU gets CPU clock time when LCD is on
        // DEBUG: Arbitrary increase wiating for display to be active
        clock += CLOCK_H_BLANK;
    }
}

/**
 * @brief      Refresh the screen
 */
void PPU::draw()
{
    // Display
    Uint32 current_ticks = SDL_GetTicks();
    if (current_ticks < last_refresh + (1000 / FPS)) {
        return;
    }

    SDL_UpdateWindowSurface(sdl_window);

    last_refresh = current_ticks;
}


/**
 * @brief      Reset the fifo
 */
void PPU::clear_fifo()
{
    pf_index = 0;
    pf_size = 0;
}


/**
 * @brief      Consume a pixel in the fifo
 * @return     Pixel poped
 */
size_t PPU::pop_pixel()
{
    size_t pixel = pixel_fifo[pf_index];
    pf_index = (pf_index + 1) % FIFO_SIZE;
    pf_size -= 1;

    return pixel;
}


/**
 * @brief      Draws a line.
 * @return     true if the screen can be refreshed
 */
bool PPU::draw_line()
{
    clear_fifo();

    pixel_type fetching_type = BG;
    uint8_t ly = mmu->get(LY);

    if (ly < LINE_Y_COUNT) {
        // Viewport position
        //uint8_t scy = mmu->get(SCY);
        uint8_t scx = mmu->get(SCX);

        // Window position
        //uint8_t wy = mmu->get(WY);
        uint8_t wx = mmu->get(WX);

        fetch(0, ly, fetching_type);

        // Drop a few pixels according to SCX to scroll LEFT/RIGHT
        for (size_t i=0; i<scx % TILE_WIDTH; i++) {
            pop_pixel();
        }

        for (size_t x=0; x<LINE_X_COUNT; x++) {
            // We want to draw the window
            if (x == wx && window_enabled) {
                info("Fetch window!\n");
                clear_fifo();
                fetching_type = WINDOW;
            }

            // Fetch next 8 pixels
            if (pf_size <= TILE_WIDTH) {
                fetch(x, ly, fetching_type);
            }

            size_t pixel = pop_pixel();

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


/**
 * @brief      Fetch 8 next pixels to display
 * @param[in]  scx   Current viewport X
 * @param[in]  scy   Current viewport Y
 * @param[in]  x     Current column to draw [0, 160[
 * @param[in]  ly    Current line to draw
 */
void PPU::fetch(size_t x, size_t ly, pixel_type type)
{
    if (type == BG) {
        fetch_bg(x, ly);
    } else if (type == WINDOW) {
        fetch_window(x, ly);
    } else {
        error("Encountered non-implemented pixel type!\n");
    }
}


/**
 * @brief      Fetch 8 next pixels to display for background
 * @param[in]  scx   Current viewport X
 * @param[in]  scy   Current viewport Y
 * @param[in]  x     Current column to draw [0, 160[
 * @param[in]  ly    Current line to draw
 */
void PPU::fetch_bg(size_t x, size_t ly)
{
    // Viewport position
    uint8_t scy = mmu->get(SCY);
    uint8_t scx = mmu->get(SCX);

    // We want the position of the viewport)
    // We add pf_size so we get the pixels needed when the fifo arrives at those pixels
    size_t viewport_x = (scx + x + pf_size) % (MAP_WIDTH * TILE_WIDTH);
    size_t viewport_y = (scy + ly) % (MAP_HEIGHT * TILE_HEIGHT);

    fetch_at(bg_map_address, bg_window_tile_data_address, viewport_x, viewport_y);
}


/**
 * @brief      Fetch 8 next pixels to display for window
 * @param[in]  wx    Window X
 * @param[in]  wy    Window Y
 * @param[in]  x     Current column to draw [0, 160[
 * @param[in]  ly    Current line to draw
 */
void PPU::fetch_window(size_t x, size_t ly)
{
    // Window position
    uint8_t wy = mmu->get(WY);
    uint8_t wx = mmu->get(WX);

    // Given the line/column being drawn and the position of the window
    // gets the position in the window to draw
    size_t window_x = x - wx;
    size_t window_y = ly - wy;

    fetch_at(window_map_address, bg_window_tile_data_address, window_x, window_y);
}


void PPU::fetch_at(
    uint16_t base_map_address,
    uint16_t base_tileset_address,
    size_t viewport_x,
    size_t viewport_y
) {
    // Index of the tile in the map
    size_t tile_x = viewport_x / TILE_WIDTH;
    size_t tile_y = viewport_y / TILE_HEIGHT;

    // Address of the tile in the map
    uint16_t map_address = base_map_address + (tile_y * MAP_WIDTH) + tile_x;

    // Tile index in the tileset
    uint8_t tile_id  = mmu->get(map_address);

    // Address of the tile in the tileset
    uint16_t tile_address = base_tileset_address + (tile_id * TILE_SIZE);

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


/**
 * @brief      Set the pre-defined palette colors to use
 * @param[in]  palette_index  0 is old shcool, same as the DMG
 *                            1 is high contrast with real black/white
 */
void PPU::set_palette(size_t palette_index)
{
    color_ldc_disabled = SDL_MapRGB(pixel_format, 150, 125, 16);

    // Fixed colors of the DMG
    if (palette_index == 1) {
        palette[0b00] = SDL_MapRGB(pixel_format, 255, 255, 255);
        palette[0b01] = SDL_MapRGB(pixel_format, 211, 211, 211);
        palette[0b10] = SDL_MapRGB(pixel_format, 120, 120, 120);
        palette[0b11] = SDL_MapRGB(pixel_format,   0,   0,   0);
    } else {
        palette[0b00] = SDL_MapRGB(pixel_format, 110, 125,  70);
        palette[0b01] = SDL_MapRGB(pixel_format,  80, 105,  75);
        palette[0b10] = SDL_MapRGB(pixel_format,  60,  90,  85);
        palette[0b11] = SDL_MapRGB(pixel_format,  60,  80,  75);
    }
}
