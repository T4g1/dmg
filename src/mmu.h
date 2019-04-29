#ifndef MMU_H
#define MMU_H

#include <inttypes.h>
#include <cstddef>

#define RAM_SIZE        0xFFFF


/**
 * @brief      Memory Management Unit
 */
class MMU {
    uint8_t ram[RAM_SIZE];

public:
    void set(uint16_t address, uint8_t value);
    void *at(uint16_t address);
    uint8_t get(uint16_t address);
    uint16_t get16(uint16_t address);
    int8_t get_signed(uint16_t address);
    bool load(const char *filepath, uint16_t dst);
    bool load(uint8_t *program, size_t size, uint16_t dst);
    bool load(uint8_t *program, size_t size);

    /**
     * @brief      Display memory status from given address ranges
     * @param[in]  start  Starting point of the dump
     * @param[in]  end    End point of the dump
     */
    void dump(uint16_t start, uint16_t end);
};

#endif /* MMU_H */
