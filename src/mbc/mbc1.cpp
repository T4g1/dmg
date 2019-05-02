#include "mbc1.h"

#include <string.h>

#include "../log.h"


MBC1::MBC1() : selected_mbc(0)
{

}


/**
 * @brief      Give access to the requested address in the correct memory bank
 * @param[in]  address  Requested address
 * @return     Address to an immutable value
 */
const void *MBC1::at(uint16_t address)
{
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

    return true;
}
