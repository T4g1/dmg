#include "cartridge.h"

#include "log.h"

#include "mbc/mbc1.h"


Cartridge::Cartridge() : mbc(nullptr)
{

}


/**
 * @brief      Load the cartridge ROM
 * @param[in]  path_rom  The path to the ROM
 * @return     true if the ROM is correctly loaded
 */
bool Cartridge::load(const char *path_rom)
{
    debug("Loading cartridge ROM: %s\n", path_rom);

    delete mbc;

    FILE *f = fopen(path_rom, "rb");
    if (f == NULL) {
        error("Unable to read provided boot ROM file\n");
        return false;
    }

    uint8_t memory_bank[MBC_SIZE];
    size_t address = 0;     //!< Address in the memory bank being read
    size_t mb_index = 0;    //!< Index of memory bank
    while (!feof(f)) {
        if (fread(&memory_bank[address++], sizeof(uint8_t), 1, f) != 1) {
            error("Error while reading provided boot ROM file\n");
            return false;
        }

        if (address != MBC_SIZE) {
            continue;
        }

        // First memory bank: We need the type of MBC to use
        if (mb_index == 0) {
            uint8_t cartridge_type = memory_bank[0x0147];

            if (cartridge_type == 0x01) {
                mbc = new MBC1();
                if (mbc == nullptr) {
                    error("Unable to allocate space for the MBC\n");
                    return false;
                }
            }
        }

        if (!mbc->load(mb_index, memory_bank)) {
            return false;
        }
    }

    // Failure of reading not EOF related
    if (!feof(f)) {
        error("Unkown error prevented the loading of the game cartridge ROM\n");
        return false;
    }

    return true;
}


/**
 * @brief      Give access to the ROM of the cartridge using the MBC
 * @param[in]  address  Requested address
 * @return     Constant pointer to the address requested
 */
const void *Cartridge::at(uint16_t address)
{
    return mbc->at(address);
}
