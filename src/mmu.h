#ifndef MMU_H
#define MMU_H

#include <inttypes.h>
#include <cstddef>

#include "cartridge.h"

#define RAM_SIZE        0xFFFF
#define ROM_START       0x0000
#define ROM_END         0x7FFF
#define BOOT_START      0x0000
#define BOOT_END        0x00FF
#define BOOT_ROM_ENABLE 0xFF50

#define TILE_ADDRESS_1      0x8000
#define TILE_ADDRESS_2      0x8800
#define MAP_ADDRESS_1       0x9800
#define MAP_ADDRESS_2       0x9C00

#define LCDC                0xFF40      // LCD Control register
#define LCD_STATUS          0xFF41      // Status of the LCD
#define SCY                 0xFF42      // Viewport Y position
#define SCX                 0xFF43      // Viewport X position
#define LY                  0xFF44      // Current line being drawn
#define LYC                 0xFF45      // Value to be compared with LY (interrupts)
#define BGP                 0xFF47      // BG Palette data
#define WY                  0xFF4A      // Window Y position
#define WX                  0xFF4B      // Window X position

enum address_type {
    BOOT,
    ROM,
    RAM,
    VRAM,
    OAM,
};


class PPU;


/**
 * @brief      Memory Map Unit
 */
class MMU {
    bool booted;
    Cartridge *cart;
    PPU *ppu;

    uint8_t ram[RAM_SIZE];
    address_type get_address_identity(uint16_t address);

public:
    MMU();

    void set_ppu(PPU *ppu);

    bool set(uint16_t address, uint8_t value);
    const void *at(uint16_t address);
    uint8_t get(uint16_t address);
    uint16_t get16(uint16_t address);
    int8_t get_signed(uint16_t address);

    bool load(const char *filepath);
    bool load(uint8_t *program, size_t size, uint16_t dst);
    bool load(uint8_t *program, size_t size);

    void dump(uint16_t start, uint16_t end);

    void set_cartridge(Cartridge *cart);

    void set_boot_rom_enable(uint8_t value);
};

#endif /* MMU_H */
