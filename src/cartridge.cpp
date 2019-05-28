#include "cartridge.h"

#include "defines.h"
#include "log.h"

#include "mbc/none.h"
#include "mbc/mbc1.h"


Cartridge::Cartridge()
{
    // Default empty random cartridge
    cartridge_type = CART_TYPE_ROM_ONLY;
    rom_type = 0;
    ram_type = 0;

    rom_path = "";
    has_battery = false;

    mbc = new NoMBC();
    mbc->init();
}


Cartridge::~Cartridge()
{
    // Save RAM content
    if (has_battery) {
        save_ram();
    }

    clear_mbc();
}


void Cartridge::clear_mbc()
{
    delete mbc;

    mbc = nullptr;
}


/**
 * @brief      Load the cartridge ROM
 * @param[in]  path_rom  The path to the ROM
 * @return     true if the ROM is correctly loaded
 */
bool Cartridge::load(std::string rom_path)
{
    info("Loading cartridge ROM: %s\n", rom_path.c_str());
    this->rom_path = rom_path;

    FILE *f = fopen(rom_path.c_str(), "rb");
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
            cartridge_type = memory_bank[CARTRIDGE_TYPE_ADDRESS];
            rom_type = memory_bank[ROM_SIZE_ADDRESS];
            ram_type = memory_bank[RAM_SIZE_ADDRESS];

            size_t rom_bank_count = Cartridge::get_rom_bank_count(rom_type);

            clear_mbc();

            switch(cartridge_type) {
            case CART_TYPE_ROM_ONLY:
                mbc = new NoMBC();
                break;
            case CART_TYPE_MBC1_RAM_BATTERY:
                has_battery = true;
                __attribute__ ((fallthrough));
            case CART_TYPE_MBC1:
            case CART_TYPE_MBC1_RAM:
                mbc = new MBC1(rom_bank_count, ram_type);
                break;
            default:
                error("Please implement cartridge type 0x%02X\n", cartridge_type);
                return false;
            }

            if (mbc == nullptr) {
                error("Unable to allocate space for the MBC\n");
                return false;
            }

            mbc->init();

            if (has_battery) {
                load_ram();
            }
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
 * @return     Value
 */
uint8_t Cartridge::get(uint16_t address)
{
    return mbc->get(address);
}


bool Cartridge::set(uint16_t address, uint8_t value)
{
    return mbc->set(address, value);
}


bool Cartridge::has_ram()
{
    return mbc->ram_size > 0;
}


std::string Cartridge::get_save_name()
{
    return rom_path + ".sav";
}


/**
 * @brief      Saves RAM content (battery like)
 */
void Cartridge::save_ram()
{
    info("Save game to %s\n", get_save_name().c_str());

    std::ofstream file;
    file.open(get_save_name(), std::ios::binary);

    file.write(reinterpret_cast<const char*>(mbc->ram), sizeof(uint8_t) * mbc->ram_size);

    file.close();
}


/**
 * @brief      Load RAM content (battery like)
 */
void Cartridge::load_ram()
{
    info("Load game from %s\n", get_save_name().c_str());

    std::ifstream file;
    file.open(get_save_name(), std::ios::binary);
    if (!file.is_open()) {
        return;
    }

    file.read(reinterpret_cast<char*>(mbc->ram), sizeof(uint8_t) * mbc->ram_size );

    file.close();
}


/**
 * @brief      Get amount of bank needed for ROM
 * @param[in]  ram_type  The ram type
 * @return     The ram size.
 */
size_t Cartridge::get_rom_bank_count(uint8_t rom_type)
{
    switch(rom_type) {
    default:
    case 0x00: return 2;
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
    }
}


/**
 * @brief      Get amount of bank needed for RAM
 * @param[in]  ram_type  The ram type
 * @return     The ram size.
 */
size_t Cartridge::get_ram_bank_count(uint8_t ram_type)
{
    switch(ram_type) {
    default: return 1;
    case 03: return 4;
    case 06: return 16;
    }
}


/**
 * @brief      Get size of RAM in kilo-bytes
 * @param[in]  ram_type  The ram type
 * @return     The ram size.
 */
size_t Cartridge::get_ram_size(uint8_t ram_type)
{
    switch(ram_type) {
    default:
    case 00: return 0;
    case 01: return 1;
    case 02: return 8;
    case 03: return 32;
    case 06: return 128;
    }
}


void Cartridge::serialize(std::ofstream &file)
{
    file.write(reinterpret_cast<char*>(&cartridge_type), sizeof(uint8_t));
    file.write(reinterpret_cast<char*>(&rom_type), sizeof(uint8_t));
    file.write(reinterpret_cast<char*>(&ram_type), sizeof(uint8_t));
    file.write(reinterpret_cast<char*>(&has_battery), sizeof(bool));

    mbc->serialize(file);
}


void Cartridge::deserialize(std::ifstream &file)
{
    clear_mbc();

    file.read(reinterpret_cast<char*>(&cartridge_type), sizeof(uint8_t));
    file.read(reinterpret_cast<char*>(&rom_type), sizeof(uint8_t));
    file.read(reinterpret_cast<char*>(&ram_type), sizeof(uint8_t));
    file.read(reinterpret_cast<char*>(&has_battery), sizeof(bool));

    size_t rom_bank_count = Cartridge::get_rom_bank_count(rom_type);

    switch(cartridge_type) {
    case CART_TYPE_ROM_ONLY:
        mbc = new NoMBC();
        break;
    case CART_TYPE_MBC1:
    case CART_TYPE_MBC1_RAM:
    case CART_TYPE_MBC1_RAM_BATTERY:
    default:
        mbc = new MBC1(rom_bank_count, ram_type);
        break;
    }

    mbc->deserialize(file);
}


