#ifndef NONE_H
#define NONE_H

#include "../mbc.h"

#define ROM_SIZE        0x8000


class NoMBC : public MBC {
    uint8_t memory[ROM_SIZE];

public:
    NoMBC();

    const void *at(uint16_t address);
    bool load(size_t mb_index, const uint8_t *rom);
    void dump(size_t mb_index, uint16_t start, uint16_t end);
};

#endif /* NONE_H */
