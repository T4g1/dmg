#include "ppu.h"

#include "dmg.h"
#include "log.h"
#include "utils.h"

PPU::PPU() : mmu(nullptr)
{
    sdl_screen = nullptr;
    sdl_window = nullptr;
}

bool PPU::init()
{
    if (mmu == nullptr) {
        error("No MMU linked with PPU\n");
        return false;
    }

    quit();

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

    reset();

    set_palette(0);

    return true;
}


void PPU::reset()
{
    lcd_enabled = false;
    window_enabled = false;
    sprites_enabled = false;
    background_enabled = false;

    clock = 0;

    current_ly = 0xFF;
}


/**
 * @brief      Emulates the PPU
 */
void PPU::step()
{
    uint8_t ly = mmu->get_nocheck(LY);

    // Reset occured
    if (ly != current_ly || ly >= MAX_LY || current_ly >= MAX_LY) {
        ly = 0;
        current_mode = H_BLANK;
    }

    // End of line draw
     else if (current_mode == H_BLANK ||
              current_mode == V_BLANK) {
        ly = (ly + 1);

        if (ly >= MAX_LY) {
            ly = 0;
        }
    }

    mmu->set_nocheck(LY, ly);
    current_ly = ly;

    // Draw (0 - 143)
    if (ly < LINE_Y_COUNT) {
        if (current_mode == H_BLANK ||
            current_mode == V_BLANK) {
            oam_search(ly);

            current_mode = OAM_SEARCH;
            update_lcd_status();
            clock += CLOCK_OAM_SEARCH;
        }

        // Pixel transfer
        else if (current_mode == OAM_SEARCH) {
            pixel_transfer(ly);

            current_mode = PIXEL_TRANSFER;
            update_lcd_status();
            clock += CLOCK_PIXEL_TRANSFER;
        }

        // H-BLANK
        else /*if (current_mode == PIXEL_TRANSFER)*/ {
            current_mode = H_BLANK;
            update_lcd_status();
            clock += CLOCK_H_BLANK;
        }
    }

    // V-Blank (144 - 153)
    else if (ly < MAX_LY) {
        // V-Blank interrupt
        if (ly == LINE_Y_COUNT) {
            mmu->trigger_interrupt(INT_V_BLANK_MASK);

            SDL_UpdateWindowSurface(sdl_window);
        }

        current_mode = V_BLANK;
        update_lcd_status();
        clock += CLOCK_V_BLANK;
    }
}


void PPU::quit()
{
    if (sdl_screen) {
        SDL_FreeSurface(sdl_screen);
    }

    if (sdl_window) {
        SDL_DestroyWindow(sdl_window);
    }

    sdl_screen = nullptr;
    sdl_window = nullptr;
}


/**
 * @brief      Handles OAM search
 * We search for at most 10 sprites that must be displayed
 */
void PPU::oam_search(uint8_t ly)
{
    displayable_sprites.clear();

    for (size_t oam_id=0; oam_id < OAM_COUNT && displayable_sprites.size() < MAX_SPRITE_DISPLAYED; oam_id++) {
        uint16_t oam_address = OAM_START + (oam_id * OAM_ENTRY_SIZE);

        Sprite sprite;
        sprite.y = mmu->get_nocheck(oam_address);
        sprite.x = mmu->get_nocheck(oam_address + 1);

        if (sprite.x != 0 &&
            sprite.y + TILE_WIDTH > ly + SPRITE_Y_OFFSET &&
            ly + SPRITE_Y_OFFSET >= sprite.y) {
            sprite.tile = mmu->get_nocheck(oam_address + 2);
            sprite.attrs = mmu->get_nocheck(oam_address + 3);

            displayable_sprites.push_back(sprite);
        }
    }
}


/**
 * @brief      Pixel transfer
 */
void PPU::pixel_transfer(uint8_t ly)
{
    clear_fifo();
    pixel_type fetching_type = BG;

    // Viewport position
    //uint8_t scy = mmu->get(SCY);
    uint8_t scx = mmu->get(SCX);

    // Window position
    uint8_t wy = mmu->get_nocheck(WY);
    uint8_t wx = mmu->get_nocheck(WX) - WINDOW_X_OFFSET;

    fetch(0, ly, fetching_type);

    // Drop a few pixels according to SCX to scroll LEFT/RIGHT
    for (size_t i=0; i<scx % TILE_WIDTH; i++) {
        pop_pixel();
    }

    for (size_t x=0; x<LINE_X_COUNT; x++) {
        // We want to draw the window
        if (window_enabled && x == wx && ly >= wy) {
            clear_fifo();
            fetching_type = WINDOW;
        }

        // Fetch next 8 pixels
        if (pf_size <= TILE_WIDTH) {
            fetch(x, ly, fetching_type);
        }

        // Fetch sprites
        if (sprites_enabled) {
            for (auto const& sprite : displayable_sprites) {
                uint8_t sprite_x_corrected = sprite.x - SPRITE_X_OFFSET;
                if (sprite_x_corrected == x) {
                    fetch_sprite(sprite, ly, TILE_WIDTH);
                }
                // Pixel partially hidden on the left
                else if (x == 0 && sprite.x < SPRITE_X_OFFSET) {
                    fetch_sprite(sprite, ly, sprite.x);
                }
            }
        }

        Pixel pixel = pop_pixel();

        Uint32 color = color_lcd_disabled;

        // Don't color anything if LCD is disabled
        if (lcd_enabled) {
            uint8_t *used_palette = bg_palette;
            if (pixel.type == SPRITE_OBP0) {
                used_palette = sprite_palette[0];
            }
            else if (pixel.type == SPRITE_OBP1) {
                used_palette = sprite_palette[1];
            }

            color = palette[used_palette[pixel.value]];
        }

        set_pixel(sdl_screen, x, ly, color);
    }
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
    uint8_t scy = mmu->get_nocheck(SCY);
    uint8_t scx = mmu->get_nocheck(SCX);

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
    uint8_t wy = mmu->get_nocheck(WY);
    uint8_t wx = mmu->get_nocheck(WX) - WINDOW_X_OFFSET;

    // Given the line/column being drawn and the position of the window
    // gets the position in the window to draw
    size_t window_x = x - wx;
    size_t window_y = ly - wy;

    fetch_at(window_map_address, bg_window_tile_data_address, window_x, window_y);
}


/**
 * @brief      Fetches a sprite.
 * @param[in]  sprite  The sprite
 * @param[in]  ly      Line to fetch
 */
void PPU::fetch_sprite(const Sprite &sprite, size_t ly, size_t pixel_count)
{
    size_t viewport_y = ly - (sprite.y - SPRITE_Y_OFFSET);

    if (get_bit(sprite.attrs, BIT_SPRITE_Y_FLIP)) {
        viewport_y = (TILE_HEIGHT - 1) - viewport_y;
    }

    // Address of the tile in the tileset
// TODO: Handle SPRITE_HEIGHT (ignore last bit)
    uint16_t tile_address = SPRITE_TILE_ADDRESS + (sprite.tile * TILE_SIZE);

    // Data for the current line of the tile being drawn
    uint16_t tile_line_address = tile_address + TILE_LINE_SIZE * (viewport_y % TILE_HEIGHT);

    uint8_t data1 = mmu->get_nocheck(tile_line_address);
    uint8_t data2 = mmu->get_nocheck(tile_line_address + 1);

    Pixel pixel;

    // Palette used
    pixel.type = SPRITE_OBP0;
    if (get_bit(sprite.attrs, BIT_SPRITE_PALETTE_NUMBER)) {
        pixel.type = SPRITE_OBP1;
    }

    // Pirority versus background
    bool sprite_on_top = true;
    if (get_bit(sprite.attrs, BIT_SPRITE_PRIORITY)) {
        sprite_on_top = false;
    }

    // Exctract the pixel_count pixels for the data (usually 8)
    for (size_t i=0; i<pixel_count; i++) {
        uint8_t index = (TILE_WIDTH - pixel_count) + i;

        if (get_bit(sprite.attrs, BIT_SPRITE_X_FLIP)) {
            pixel.value = get_pixel_value(data1, data2, 7 - index);
        } else {
            pixel.value = get_pixel_value(data1, data2, index);
        }

        Pixel current_pixel = pixel_fifo[(pf_index + i) % FIFO_SIZE];

        // Sprite with lowest X is on top
        if (current_pixel.type == SPRITE_OBP0 ||
            current_pixel.type == SPRITE_OBP1 ||
        // 00 is transparent for sprite
            pixel.value == 0) {
            continue;
        }

        // BG 0 is always behind sprites
        if (!sprite_on_top && current_pixel.value != 0) {
            continue;
        }

        pixel_fifo[(pf_index + i) % FIFO_SIZE] = pixel;
    }
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
    uint8_t tile_id = mmu->get_nocheck(map_address);
    if (base_tileset_address == TILE_ADDRESS_2) {
        // When tileset base address is $8800: tile ID are signed and 0 is $9000
        tile_id = mmu->get_signed(map_address) + 128;
    }

    // Address of the tile in the tileset
    uint16_t tile_address = base_tileset_address + (tile_id * TILE_SIZE);

    // Data for the current line of the tile being drawn
    uint16_t tile_line_address = tile_address + TILE_LINE_SIZE * (viewport_y % TILE_HEIGHT);

    uint8_t data1 = mmu->get_nocheck(tile_line_address);
    uint8_t data2 = mmu->get_nocheck(tile_line_address + 1);

    // Exctract the 8 pixels for the data
    for (size_t i=0; i<TILE_WIDTH; i++) {
        Pixel pixel;
        pixel.value = get_pixel_value(data1, data2, i);
        pixel.type = BG;    // Only used for palette so same as WINDOW here

        pixel_fifo[(pf_index + pf_size++) % FIFO_SIZE] = pixel;
    }
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
Pixel PPU::pop_pixel()
{
    Pixel pixel = pixel_fifo[pf_index];
    pf_index = (pf_index + 1) % FIFO_SIZE;
    pf_size -= 1;

    return pixel;
}


/**
 * @brief      Set LCD status according to current mode and coincidence flag
 */
void PPU::update_lcd_status()
{
    uint8_t old_status = mmu->get(LCD_STATUS);
    uint8_t new_status = old_status;

    uint8_t lyc = mmu->get(LYC);

    bool coincidence = current_ly == lyc;

    new_status = set_bit(new_status, BIT_COINCIDENCE_LY_LYC, coincidence);

    // Clear then set mode
    new_status &= ~MASK_MODE;
    new_status |= (current_mode & MASK_MODE);

    mmu->set_nocheck(LCD_STATUS, new_status);

    update_interrupts(old_status, new_status);
}


/**
 * @brief      Set interrupt flags as needed
 * @param[in]  old_status  The old status
 * @param[in]  new_status  The new status
 */
void PPU::update_interrupts(uint8_t old_status, uint8_t new_status)
{
    // LYC == LY interrupt
    if (get_bit(new_status, BIT_INT_COINCIDENCE)) {
        bool old_coincidence = get_bit(old_status, BIT_COINCIDENCE_LY_LYC);
        bool new_coincidence = get_bit(new_status, BIT_COINCIDENCE_LY_LYC);
        bool coincidence_triggerred = old_coincidence != new_coincidence;
        if (new_coincidence && coincidence_triggerred) {
            mmu->trigger_interrupt(INT_LCD_STAT_MASK);
        }
    }
    // OAM interrupt
    if (get_bit(new_status, BIT_INT_OAM)) {
        if (current_mode == OAM_SEARCH &&
            (old_status & MASK_MODE) != OAM_SEARCH) {
            mmu->trigger_interrupt(INT_LCD_STAT_MASK);
        }
    }
    // V-BLANK interrupt
    if (get_bit(new_status, BIT_INT_V_BLANK)) {
        if (current_mode == V_BLANK &&
            (old_status & MASK_MODE) != V_BLANK) {
            mmu->trigger_interrupt(INT_LCD_STAT_MASK);
        }
    }
    // H-BLANK interrupt
    if (get_bit(new_status, BIT_INT_H_BLANK)) {
        if (current_mode == H_BLANK &&
            (old_status & MASK_MODE) != H_BLANK) {
            mmu->trigger_interrupt(INT_LCD_STAT_MASK);
        }
    }
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

    sprite_height = 8;
    if (get_bit(lcdc, BIT_SPRITES_SIZE)) {
        sprite_height = 16;
    }
}


/**
 * @brief      Set the palette used for background tiles
 * @param[in]  bgp  BGP value
 */
void PPU::set_bgp(uint8_t value)
{
    bg_palette[0] = (value & 0b00000011);
    bg_palette[1] = (value & 0b00001100) >> 2;
    bg_palette[2] = (value & 0b00110000) >> 4;
    bg_palette[3] = (value & 0b11000000) >> 6;
}


/**
 * @brief      Set the palette used for sprites
 * @param[in]  value  Palette value
 */
void PPU::set_obp(size_t obp_id, uint8_t value)
{
    sprite_palette[obp_id][0] = (value & 0b00000011);
    sprite_palette[obp_id][1] = (value & 0b00001100) >> 2;
    sprite_palette[obp_id][2] = (value & 0b00110000) >> 4;
    sprite_palette[obp_id][3] = (value & 0b11000000) >> 6;
}


Uint32 PPU::get_window_id()
{
    return SDL_GetWindowID(sdl_window);
}


uint8_t PPU::get_current_ly()
{
    return current_ly;
}


const char *PPU::get_current_mode()
{
    switch(current_mode) {
    case OAM_SEARCH: return "OAM Search";
    case PIXEL_TRANSFER: return "Pixel transfer";
    case H_BLANK: return "H-Blank";
    default:
    case V_BLANK: return "V-Blank";
    }

    return "";
}


/**
 * @brief      Set the pre-defined palette colors to use
 * @param[in]  palette_index  0 is old shcool, same as the DMG
 *                            1 is high contrast with real black/white
 */
void PPU::set_palette(size_t palette_index)
{
    color_lcd_disabled = SDL_MapRGB(pixel_format, 150, 125, 16);

    // Blac/White
    if (palette_index == 1) {
        palette[0] = SDL_MapRGB(pixel_format, 255, 255, 255);
        palette[1] = SDL_MapRGB(pixel_format, 211, 211, 211);
        palette[2] = SDL_MapRGB(pixel_format, 120, 120, 120);
        palette[3] = SDL_MapRGB(pixel_format,   0,   0,   0);
    }
    // Clean green scales
    else if (palette_index == 2) {
        palette[0] = SDL_MapRGB(pixel_format, 0xE0, 0xF8, 0xD0);
        palette[1] = SDL_MapRGB(pixel_format, 0x88, 0xC0, 0x70);
        palette[2] = SDL_MapRGB(pixel_format, 0x34, 0x68, 0x56);
        palette[3] = SDL_MapRGB(pixel_format, 0x08, 0x18, 0x20);
    }
    // Original values
    else {
        palette[0] = SDL_MapRGB(pixel_format, 110, 125,  70);
        palette[1] = SDL_MapRGB(pixel_format,  80, 105,  75);
        palette[2] = SDL_MapRGB(pixel_format,  60,  90,  85);
        palette[3] = SDL_MapRGB(pixel_format,  60,  80,  75);
    }
}


void PPU::set_mmu(MMU *mmu)
{
    this->mmu = mmu;
}


/**
 * @brief      Draw each tile information as a RGB float for OpenGL in the given buffer
 * Assumes buffer is TILE_WIDTH * TILE_HEIGHT * 3 (RGB) in size
 * @param      buffer   The buffer
 * @param[in]  tile_id  The tile identifier
 */
void PPU::draw_tile(uint8_t buffer[], size_t tile_id)
{
    uint16_t tile_address = TILE_ADDRESS_1 + (tile_id * TILE_SIZE);

    for (size_t y=0; y<TILE_HEIGHT; y++) {
        uint16_t line_address = tile_address + (y * TILE_LINE_SIZE);

        uint8_t data1 = mmu->get_nocheck(line_address);
        uint8_t data2 = mmu->get_nocheck(line_address + 1);

        for (size_t x=0; x<TILE_WIDTH; x++) {
            uint8_t pixel = get_pixel_value(data1, data2, x);

            size_t index = (y * TILE_WIDTH) + x;
            index *= 3;     // Three color component

            SDL_GetRGB(
                palette[pixel],
                pixel_format,
                &buffer[index + 0],
                &buffer[index + 1],
                &buffer[index + 2]
            );
        }
    }
}
