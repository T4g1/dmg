#ifndef MMU_H
#define MMU_H

#include <inttypes.h>
#include <cstddef>

#include "cartridge.h"

#define RAM_SIZE            0xFFFF
#define ROM_START           0x0000
#define ROM_END             0x7FFF
#define MBC0_END            0x3FFF
#define ROM_SWITCH_START    0x4000
#define BOOT_START          0x0000
#define BOOT_END            0x00FF
#define BOOT_SIZE           0x0100
#define BOOT_ROM_ENABLE     0xFF50
#define ECHO_START          0xE000
#define ECHO_END            0xFDFF
#define OAM_START           0xFE00
#define OAM_END             0xFE9F
#define CRASH_START         0xFEA0
#define CRASH_END           0xFEFF

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
    ECHO,
    CRASH,
};


class PPU;


/**
 * @brief      Memory Map Unit
 */
class MMU {
    bool booted;
    Cartridge *cart;
    PPU *ppu;

    address_type get_address_identity(uint16_t address);

public:
    uint8_t boot[BOOT_SIZE];
    uint8_t ram[RAM_SIZE];
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

    void set_cartridge(Cartridge *cart);
    void set_boot_rom_enable(uint8_t value);

    bool is_booted();

    void update_ram();
};

#endif /* MMU_H */
