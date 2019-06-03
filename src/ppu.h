#ifndef PPU_H
#define PPU_H

#include <SDL2/SDL.h>
#include <list>
#include <iostream>
#include <fstream>

#include "defines.h"
#include "mmu.h"


/**
 * @brief      Pixel Processing Unit
 */
class PPU {
public:
    size_t clock;

    PPU();
    ~PPU();

    bool init();
    void reset();
    void step();
    void quit();

    void set_lcdc(uint8_t lcdc);
    void set_bgp(uint8_t value);
    void set_obp(size_t obp_id, uint8_t value);

    Uint32 get_window_id();
    uint8_t get_current_ly();
    const char *get_current_mode();

    void set_palette(size_t palette_index);
    void set_mmu(MMU *mmu);

    void draw_tile(uint8_t buffer[], size_t tile_id);

    void adjust_clocks(size_t adjustment);

    void serialize(std::ofstream &file);
    void deserialize(std::ifstream &file);

private:
    MMU *mmu;

    SDL_Window *sdl_window;
    SDL_Surface *sdl_screen;
    SDL_PixelFormat *pixel_format;

    Uint32 color_lcd_disabled;
    Uint32 palette[PALETTE_SIZE];

    uint8_t bg_palette[PALETTE_SIZE];
    uint8_t sprite_palette[SPRITE_PALETTE_COUNT][PALETTE_SIZE];

    bool lcd_enabled;
    bool window_enabled;
    bool sprites_enabled;
    bool background_enabled;

    size_t sprite_height;

    uint16_t bg_window_tile_data_address;
    uint16_t bg_map_address;
    uint16_t window_map_address;

    Pixel pixel_fifo[FIFO_SIZE];
    size_t pf_size;             // How many pixels in the FIFO
    size_t pf_index;            // Position in the FIFO

    uint8_t current_ly;                     // Line that was being drawn
    ppu_mode current_mode;
    std::list<Sprite> displayable_sprites;  // Contains results of OAM search

    void oam_search(uint8_t ly);
    void pixel_transfer(uint8_t ly);

    void fetch(size_t x, size_t ly, pixel_type type);
    void fetch_bg(size_t x, size_t ly);
    void fetch_window(size_t x, size_t ly);
    void fetch_sprite(const Sprite &sprite, size_t ly, size_t pixel_count);
    void fetch_at(
        uint16_t map_address, uint16_t tileset_address,
        size_t viewport_x, size_t viewport_y);

    void clear_fifo();
    Pixel pop_pixel();

    void update_lcd_status();
    void update_interrupts(uint8_t old_status, uint8_t new_status);

    uint8_t get_wx();

    friend class Debugger;
};

#endif /* PPU_H */
