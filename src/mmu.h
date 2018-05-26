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
    uint16_t get16(uint16_t address);
    int8_t get_signed(uint16_t address);
    bool load(const char *filepath, uint16_t dst);
    void dump(uint16_t start, uint16_t end);
};

#endif /* MMU_H */
