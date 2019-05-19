#ifndef NONE_H
#define NONE_H

#include "../mbc.h"

#define ROM_SIZE        0x8000


class NoMBC : public MBC {
public:
    NoMBC();

    void init();
    const void *at(uint16_t address);
    bool set(uint16_t address, uint8_t value);
    bool load(size_t mb_index, const uint8_t *rom);
};

#endif /* NONE_H */
