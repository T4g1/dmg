#ifndef MMU_H
#define MMU_H

#include <inttypes.h>

#define RAM_SIZE        0xFFFF


class MMU {
    uint8_t ram[RAM_SIZE];

public:
    void set(uint16_t address, uint8_t value);
    void *at(uint16_t address);
    uint8_t get(uint16_t address);
    bool load(const char *filepath, uint16_t dst);
};

#endif /* MMU_H */
