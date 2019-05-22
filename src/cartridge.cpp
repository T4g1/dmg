#include "cartridge.h"

#include "log.h"

#include "mbc/none.h"
#include "mbc/mbc1.h"


Cartridge::Cartridge()
{
    // Default empty random cartridge
    mbc = new NoMBC();
    mbc->init();
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
        error("Unable to read provided cartridge ROM file\n");
        return false;
    }

    uint8_t memory_bank[MBC_SIZE];
    size_t address = 0;     //!< Address in the memory bank being read
    size_t mb_index = 0;    //!< Index of memory bank
    while (fread(&memory_bank[address++], sizeof(uint8_t), 1, f) == 1) {
        if (address != MBC_SIZE) {
            continue;
        }

        // First memory bank: We need the type of MBC to use
        if (mb_index == 0) {
            uint8_t cartridge_type = memory_bank[CARTRIDGE_TYPE_ADDRESS];
            uint8_t rom_type = memory_bank[ROM_SIZE_ADDRESS];

            if (cartridge_type == CART_TYPE_ROM_ONLY) {
                mbc = new NoMBC();
            }

            else if (cartridge_type == CART_TYPE_MBC1 ||
                     cartridge_type == CART_TYPE_MBC1_RAM ||
                     cartridge_type == CART_TYPE_MBC1_RAM_BATTERY) {
                size_t bank_count = Cartridge::get_bank_count(rom_type);
                mbc = new MBC1(bank_count);
            }

            else {
                error("Please implement cartridge type 0x%02X\n", cartridge_type);
                return false;
            }

            if (mbc == nullptr) {
                error("Unable to allocate space for the MBC\n");
                return false;
            }

            mbc->init();
        }

        if (!mbc->load(mb_index, memory_bank)) {
            return false;
        }

        mb_index += 1;
        address = 0;
    }

    // Failure of reading not EOF related
    if (!feof(f)) {
        error("Unkown error prevented the loading of the game cartridge ROM\n");
        debug("MB index: %zu, address %zu\n", mb_index, address);
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


bool Cartridge::set(uint16_t address, uint8_t value)
{
    return mbc->set(address, value);
}

size_t Cartridge::get_bank_count(uint8_t rom_type)
{
    switch(rom_type) {
    case 0x01: return 4;
    case 0x02: return 8;
    case 0x03: return 16;
    case 0x04: return 32;
    case 0x05: return 64;
    case 0x06: return 128;
    case 0x07: return 256;
    case 0x52: return 72;
    case 0x53: return 80;
    case 0x54: return 96;
    default: return 0;
    }

    return 0;
}


