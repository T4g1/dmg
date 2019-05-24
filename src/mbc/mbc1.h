#ifndef MBC1_H
#define MBC1_H

#include "../mbc.h"


#define RAM_ENABLE_START            0x0000
#define RAM_ENABLE_END              0x1FFF
#define ROM_BANK_NUMBER_START       0x2000
#define ROM_BANK_NUMBER_END         0x3FFF
#define RAM_BANK_NUMBER_START       0x4000
#define RAM_BANK_NUMBER_END         0x5FFF
#define ROM_RAM_MODE_SELECT_START   0x6000
#define ROM_RAM_MODE_SELECT_END     0x7FFF

#define LOWER_ROM_BANK_NUMBER_MASK  0b00011111
#define UPPER_ROM_BANK_NUMBER_MASK  0b00000011


/**
 * @brief      First Memory Bank Controller created
 *
 * Give access to 4 memory banks (32Kbits of ROM).
 */
class MBC1 : public MBC {
    size_t mbc_count;

    bool rom_mode_selected;

    uint8_t rom_bank_select;
    uint8_t ram_bank_select;

    size_t get_selected_rom_bank();

public:
    MBC1(size_t bank_count);
    ~MBC1();

    void init();
    uint8_t get(uint16_t address);
    bool set(uint16_t address, uint8_t value);
    bool load(size_t mb_index, const uint8_t *rom);
};

#endif /* MBC1_H */
