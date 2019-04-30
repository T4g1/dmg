#include <stdio.h>
#include <string.h>

#include "utils.h"
#include "log.h"

#include "mmu.h"


void MMU::set(uint16_t address, uint8_t value)
{
    ram[address] = value;
}


void *MMU::at(uint16_t address)
{
    return ram + address;
}


uint8_t MMU::get(uint16_t address)
{
    return *(uint8_t*) at(address);
}


uint16_t MMU::get16(uint16_t address)
{
    uint16_t low = (uint16_t)get(address);
    uint16_t high = (uint16_t)get(address + 1);

    return (high << 8) + low;
}


int8_t MMU::get_signed(uint16_t address)
{
    return get(address);
}


bool MMU::load(const char *filepath, uint16_t dst)
{
    debug("Loading: %s\n", filepath);

    FILE *f = fopen(filepath, "rb");
    if (f == NULL) {
        error("Unable to read provided boot ROM file\n");
        return false;
    }

    uint8_t byte;
    while (fread(&byte, sizeof(uint8_t), 1, f) == 1) {
        set(dst++, byte);
    }

    // Failure of reading not EOF related
    if (!feof(f)) {
        error("Error while reading provided boot ROM file\n");
        return false;
    }

    return true;
}


bool MMU::load(uint8_t *program, size_t size, uint16_t dst)
{
    for (size_t i=0; i<size; i++) {
        ram[dst + i] = program[i];
    }

    return true;
}


bool MMU::load(uint8_t *program, size_t size)
{
    return load(program, size, 0x0000);
}


/**
 * @brief      Display memory status from given address ranges
 * @param[in]  start  Starting point of the dump
 * @param[in]  end    End point of the dump
 */
void MMU::dump(uint16_t start, uint16_t end)
{
    info("---------------------------[ MMU ]---------------------------\n");
    size_t width = 16;

    for (size_t i=start; i<=end; i++) {
        if (i % width == 0) {
            info("0x%04X-0x%04X ", (int)i, (int)(i + width - 1));
        }
        info("%02X ", get(i));

        if (i % width == width - 1) {
            info("\n");
        }
    }
    info("\n");
}
