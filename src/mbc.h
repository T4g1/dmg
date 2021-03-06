#ifndef MBC_H
#define MBC_H

#include <stdint.h>
#include <stddef.h>
#include <iostream>
#include <fstream>


/**
 * @brief      Abstract class to Memory Bank controllers
 *
 * They are responsible of managing internal memory of cartridge and provide
 * switchable memory banks.
 */
class MBC {
public:
    uint8_t *memory;
    uint8_t *ram;

    size_t ram_size;

    virtual ~MBC() = default;

    virtual void init() = 0;
    virtual uint8_t get(uint16_t address) = 0;
    virtual bool set(uint16_t address, uint8_t value) = 0;
    virtual bool load(size_t mb_index, const uint8_t *rom) = 0;

    virtual void serialize(std::ofstream &file) = 0;
    virtual void deserialize(std::ifstream &file) = 0;
};

#endif /* MBC_H */
