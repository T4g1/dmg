#ifndef MBC1_H
#define MBC1_H

#define MBC_COUNT       4

#include "../mbc.h"


/**
 * @brief      First Memory Bank Controller created
 *
 * Give access to 4 memory banks (32Kbits of ROM).
 */
class MBC1 : public MBC {
    size_t selected_mbc;
    uint8_t mbc[MBC_COUNT][MBC_SIZE];

public:
    MBC1();

    const void *at(uint16_t address);
    bool load(size_t mb_index, const uint8_t *rom);
};

#endif /* MBC1_H */
