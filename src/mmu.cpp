#include <stdio.h>
#include <string.h>

#include "utils.h"

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

int8_t MMU::get_signed(uint16_t address)
{
    return get(address);
}

bool MMU::load(const char *filepath, uint16_t dst)
{
    fprintf(stdout, "Loading: %s\n", filepath);

    FILE *f = fopen(filepath, "rb");
    if (f == NULL) {
        fprintf(stderr, "Unable to read provided boot ROM file\n");
        return false;
    }

    uint8_t byte;
    while (fread(&byte, sizeof(uint8_t), 1, f) == 1) {
        set(dst++, byte);
    }

    // Failure of reading not EOF related
    if (!feof(f)) {
        fprintf(stderr, "Error while reading provided boot ROM file\n");
        return false;
    }

    return true;
}
