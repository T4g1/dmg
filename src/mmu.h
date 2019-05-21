#ifndef MMU_H
#define MMU_H

#include <inttypes.h>
#include <cstddef>

#include "cartridge.h"

#define RAM_SIZE            0x10000
#define BOOT_SIZE           0x0100
#define OAM_SIZE            0xA0

// Memory locations addresses
#define BOOT_START          0x0000
#define BOOT_END            0x00FF
#define ROM_START           0x0000
#define ROM_END             0x7FFF
#define ROM0_START          0x0000
#define ROM0_END            0x3FFF
#define ROM1_START          0x4000
#define ROM1_END            0x7FFF
#define VRAM_START          0x8000
#define VRAM_END            0x9FFF
#define SRAM_START          0xA000
#define SRAM_END            0xBFFF
#define WRAM0_START         0xC000
#define WRAM0_END           0xCFFF
#define WRAM1_START         0xD000
#define WRAM1_END           0xDFFF
#define ECHO_START          0xE000
#define ECHO_END            0xFDFF
#define OAM_START           0xFE00
#define OAM_END             0xFE9F
#define CRASH_START         0xFEA0
#define CRASH_END           0xFEFF
#define IO_START            0xFF00
#define IO_END              0xFF7F
#define HRAM_START          0xFF80
#define HRAM_END            0xFFFF

#define TILE_ADDRESS_1      0x8000
#define TILE_ADDRESS_2      0x8800
#define MAP_ADDRESS_1       0x9800
#define MAP_ADDRESS_2       0x9C00

#define SPRITE_TILE_ADDRESS TILE_ADDRESS_1


// Registers addresses
#define LCDC                0xFF40      // LCD Control register
#define LCD_STATUS          0xFF41      // Status of the LCD
#define SCY                 0xFF42      // Viewport Y position
#define SCX                 0xFF43      // Viewport X position
#define LY                  0xFF44      // Current line being drawn
#define LYC                 0xFF45      // Value to be compared with LY (interrupts)
#define OAM_TRANSFER        0xFF46      // Writting here starts an OAM transfers
#define BGP                 0xFF47      // BG Palette data
#define OBP0                0xFF48      // Sprite palette 0
#define OBP1                0xFF49      // Sprite palette 1
#define WY                  0xFF4A      // Window Y position
#define WX                  0xFF4B      // Window X position
#define BOOT_ROM_ENABLE     0xFF50      // Booted status
#define JOYPAD              0xFF00      // Joypad
#define DIV                 0xFF04      // Divider (timer)
#define TIMA                0xFF05      // Timer counter
#define TMA                 0xFF06      // Timer modulo
#define TAC                 0xFF07      // Timer control
#define IF_ADDRESS          0xFF0F      // Interrupt Flag
#define IE_ADDRESS          0xFFFF      // Interrupt Enable

enum address_type {
    BOOT,
    ROM0,
    ROM1,
    VRAM,
    SRAM,
    WRAM0,
    WRAM1,
    ECHO,
    OAM,
    CRASH,
    IO_RAM,
    HRAM
};


class PPU;
class Timer;
class Input;


/**
 * @brief      Memory Map Unit
 */
class MMU {
    bool booted;
    Cartridge *cart;
    PPU *ppu;
    Timer *timer;
    Input *input;

    address_type get_address_identity(uint16_t address);

public:
    uint8_t boot[BOOT_SIZE];
    uint8_t ram[RAM_SIZE];
    MMU();

    bool init(const char *path_bios, Cartridge *cart);
    void reset();
    void handle_callbacks(uint16_t address, uint8_t value);

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

    void set_booted(bool value);
    bool is_booted();

    void update_ram();

    const char *display_address_identity(uint16_t address);

    void set_ppu(PPU *ppu);
    void set_timer(Timer *timer);
    void set_input(Input *input);
};

#endif /* MMU_H */
