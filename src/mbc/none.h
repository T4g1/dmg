#ifndef NONE_H
#define NONE_H

#include "../mbc.h"


class NoMBC : public MBC {
public:
    NoMBC();

    void init();
    uint8_t get(uint16_t address);
    bool set(uint16_t address, uint8_t value);
    bool load(size_t mb_index, const uint8_t *rom);
};

#endif /* NONE_H */
