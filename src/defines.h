#ifndef DEFINES_H
#define DEFINES_H

#include <inttypes.h>

// Emulation settings
#define FPS                 30
#define GLSL_VERSION        "#version 130"
#define FRAME_CYCLES        70244       //<! Amount of CPU cycle between each frame
#define DEFAULT_SPEED       10          // Speed of the emulator

// CPU constants
#define MAX_OPCODES         256
#define REGISTER_COUNT      10  //<! 8x8-bit for standard registers + 2x8-bit for SP (16-bit)

// Flags
#define FZ               7  //<! Zero Flag
#define FN               6  //<! Add/Sub Flag
#define FH               5  //<! Half Carry Flag
#define FC               4  //<! Carry Flag

// Interrupts
#define INT_V_BLANK_MASK     0b00000001
#define INT_LCD_STAT_MASK    0b00000010
#define INT_TIMER_MASK       0b00000100
#define INT_SERIAL_MASK      0b00001000
#define INT_JOYPAD_MASK      0b00010000

// Memory sizes
#define RAM_SIZE            0x10000 // 64kB
#define ROM_SIZE            0x8000  // 32kB
#define BOOT_SIZE           0x0100
#define OAM_SIZE            0xA0
#define MBC_SIZE            0x4000  // 16kB

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
#define JOYPAD              0xFF00      // Joypad
#define SB                  0xFF01      // Serial transfert data
#define SC                  0xFF02      // Serial transfer control
#define DIV                 0xFF04      // Divider (timer)
#define TIMA                0xFF05      // Timer counter
#define TMA                 0xFF06      // Timer modulo
#define TAC                 0xFF07      // Timer control
#define IF_ADDRESS          0xFF0F      // Interrupt Flag
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
#define IE_ADDRESS          0xFFFF      // Interrupt Enable

#define CARTRIDGE_TYPE_ADDRESS          0x0147
#define ROM_SIZE_ADDRESS                0x0148
#define RAM_SIZE_ADDRESS                0x0149

// Screen constants
#define LINE_Y_COUNT        144
#define LINE_X_COUNT        160

#define SCREEN_WIDTH            LINE_X_COUNT
#define SCREEN_HEIGHT           LINE_Y_COUNT

#define OAM_ENTRY_SIZE          4       // Size in byte of an OAM entry
#define OAM_COUNT               40      // How many sprites in OAM max?
#define MAX_SPRITE_DISPLAYED    10      // Do not display more than X sprites on the same line

// LCD Controller
#define BIT_LCD_ENABLED                     7
#define BIT_WINDOW_MAP_SELECT               6
#define BIT_WINDOW_ENABLED                  5
#define BIT_BG_WINDOW_TILE_DATA_SELECT      4
#define BIT_BG_MAP_SELECT                   3
#define BIT_SPRITES_SIZE                    2
#define BIT_SPRITES_ENABLED                 1
#define BIT_BACKGROUND_ENABLED              0

// OAM Attribute
#define BIT_SPRITE_PRIORITY                 7
#define BIT_SPRITE_Y_FLIP                   6
#define BIT_SPRITE_X_FLIP                   5
#define BIT_SPRITE_PALETTE_NUMBER           4

// LCD Status
#define BIT_INT_COINCIDENCE                 6
#define BIT_INT_OAM                         5
#define BIT_INT_V_BLANK                     4
#define BIT_INT_H_BLANK                     3
#define BIT_COINCIDENCE_LY_LYC              2
#define MASK_MODE                           0b00000011

#define PALETTE_SIZE            4       // How many colors available
#define SPRITE_PALETTE_COUNT    2       // OBP0 and OBP1

#define V_BLANK_PERIOD      10
#define MAX_LY              LINE_Y_COUNT + V_BLANK_PERIOD

#define SPRITE_X_OFFSET     8       // Sprite is hidden until X >= 8
#define SPRITE_Y_OFFSET     16      // Sprite is hidden until Y >= 16
#define WINDOW_X_OFFSET     7       // Window is offseted on X axis

// Clock timing
#define CLOCK_OAM_SEARCH        20 * 4
#define CLOCK_PIXEL_TRANSFER    43 * 4
#define CLOCK_H_BLANK           51 * 4
#define CLOCK_V_BLANK           CLOCK_OAM_SEARCH + CLOCK_PIXEL_TRANSFER + CLOCK_H_BLANK

#define MAP_WIDTH            32
#define MAP_HEIGHT           32

#define PIXEL_SIZE              2      // Size of a pixel data in bit

#define TILE_HEIGHT             8
#define TILE_WIDTH              8
#define TILE_LINE_SIZE          2       // Size of a line of a tile in bytes

// Size of a tile in memory in bytes. Should be 0x10
#define TILE_SIZE               TILE_HEIGHT * TILE_LINE_SIZE

// Size of the FIFO for the PPU
#define FIFO_SIZE               16

// Cartridge
#define CART_TYPE_ROM_ONLY              0x00
#define CART_TYPE_MBC1                  0x01
#define CART_TYPE_MBC1_RAM              0x02
#define CART_TYPE_MBC1_RAM_BATTERY      0x03

// Inputs
#define SELECT_BUTTON_KEY_MASKS       0b00100000
#define SELECT_DIRECTION_KEY_MASKS    0b00010000
#define KEY_DOWN_START      3
#define KEY_UP_SELECT       2
#define KEY_LEFT_B          1
#define KEY_RIGHT_A         0

// Timer Control masks
#define TIMER_START         0b00000100
#define INPUT_CLOCK         0b00000011

#define DIV_FREQUENCY       256


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


enum pixel_type {
    BG,
    WINDOW,
    SPRITE_OBP0,
    SPRITE_OBP1
};


enum ppu_mode {
    H_BLANK = 0,
    V_BLANK,
    OAM_SEARCH,
    PIXEL_TRANSFER,
};


struct Pixel {
    uint8_t value;
    pixel_type type;
};


struct Sprite {
    uint8_t x;
    uint8_t y;
    uint8_t tile;
    uint8_t attrs;
};

#endif /* DEFINES_H */
