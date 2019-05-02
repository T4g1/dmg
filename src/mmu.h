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

enum address_type {
    BOOT,
    ROM,
    RAM
};


/**
 * @brief      Memory Map Unit
 */
class MMU {
    bool booted;
    Cartridge *cart;

    uint8_t ram[RAM_SIZE];
    address_type get_address_identity(uint16_t address);

public:
    MMU();

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
};

#endif /* MMU_H */
