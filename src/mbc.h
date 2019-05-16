#ifndef MBC_H
#define MBC_H

#include <stdint.h>
#include <stddef.h>

#define MBC_SIZE        0x4000  // 16Kb


/**
 * @brief      Abstract class to Memory Bank controllers
 *
 * They are responsible of managing internal memory of cartridge and provide
 * switchable memory banks.
 */
class MBC {
public:
    virtual ~MBC() = default;

    virtual const void *at(uint16_t address) = 0;
    virtual bool set(uint16_t address, uint8_t value) = 0;
    virtual bool load(size_t mb_index, const uint8_t *rom) = 0;
    virtual void dump(size_t mb_index, uint16_t start, uint16_t end) = 0;
};

#endif /* MBC_H */
