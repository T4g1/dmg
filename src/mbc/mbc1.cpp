#include "mbc1.h"

#include <string.h>

#include "../mmu.h"
#include "../log.h"


MBC1::MBC1() : selected_mbc(1)
{

}


/**
 * @brief      Give access to the requested address in the correct memory bank
 * @param[in]  address  Requested address
 * @return     Address to an immutable value
 */
const void *MBC1::at(uint16_t address)
{
    if (address <= MBC0_END) {
        return &mbc[0][address % MBC_SIZE];
    }

    return &mbc[selected_mbc][address % MBC_SIZE];
}


/**
 * @brief      Stores the given 16k of ROM into the MBC at the given index
 * @param[in]  mb_index  The memory bank index
 * @param[in]  rom        The rom
 * @return     true if loaded with success
 */
bool MBC1::load(size_t mb_index, const uint8_t *rom)
{
    if (mb_index >= MBC_COUNT) {
        error("MBC1 have at most %d memory bank(s)\n", MBC_COUNT);
        return false;
    }

    memcpy(mbc[mb_index], rom, MBC_SIZE);

    //dump(mb_index, 0x0000, MBC_SIZE);

    return true;
}


/**
 * @brief      Display memory status from given address ranges
 * @param[in]  start  Starting point of the dump
 * @param[in]  end    End point of the dump
 */
void MBC1::dump(size_t mb_index, uint16_t start, uint16_t end)
{
    info("-------------------------[ MBC1 %02zu ]-------------------------\n", mb_index);
    size_t width = 16;

    for (size_t i=start; i<=end; i++) {
        if (i % width == 0) {
            info("0x%04X-0x%04X ", (int)i, (int)(i + width - 1));
        }
        info("%02X ", mbc[mb_index][i % MBC_SIZE]);

        if (i % width == width - 1) {
            info("\n");
        }
    }
    info("\n");
}


bool MBC1::set(uint16_t /*address*/, uint8_t value)
{
    // We add 1 because to select memory bank 1, we write 0
    value = (value + 1) % MBC_COUNT;

    debug("Set MBC to 0x%02X\n", value);
    selected_mbc = value;

    return true;
}
