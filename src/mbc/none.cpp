#include "none.h"

#include <string.h>

#include "../log.h"


NoMBC::NoMBC()
{

}


const void *NoMBC::at(uint16_t address)
{
    return &memory[address % ROM_SIZE];
}


bool NoMBC::load(size_t mb_index, const uint8_t *rom)
{
    memcpy(this->memory + (MBC_SIZE * mb_index), rom, MBC_SIZE);

    //dump(mb_index, 0x0000, MBC_SIZE);

    return true;
}


/**
 * @brief      Display memory status from given address ranges
 * @param[in]  start  Starting point of the dump
 * @param[in]  end    End point of the dump
 */
void NoMBC::dump(size_t /*mb_index*/, uint16_t start, uint16_t end)
{
    info("----------------------------[ ROM  ]----------------------------\n");
    size_t width = 16;

    for (size_t i=start; i<=end; i++) {
        if (i % width == 0) {
            info("0x%04X-0x%04X ", (int)i, (int)(i + width - 1));
        }
        info("%02X ", memory[i % ROM_SIZE]);

        if (i % width == width - 1) {
            info("\n");
        }
    }
    info("\n");
}

