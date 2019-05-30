#include "mmu.h"

#include <stdio.h>
#include <string.h>

#include "utils.h"
#include "log.h"
#include "ppu.h"
#include "timer.h"
#include "input.h"
#include "apu.h"
#include "gui/debugger.h"


MMU::MMU() : ppu(nullptr), timer(nullptr), input(nullptr), apu(nullptr), debugger(nullptr)
{
    cart = new Cartridge();
}


MMU::~MMU()
{
    delete cart;
}


bool MMU::init(std::string boot_path, std::string rom_path)
{
    if (ppu == nullptr) {
        error("No PPU linked with MMU\n");
        return false;
    }

    if (timer == nullptr) {
        error("No Timer linked with MMU\n");
        return false;
    }

    if (input == nullptr) {
        error("No Input linked with MMU\n");
        return false;
    }

    if (apu == nullptr) {
        error("No APU linked with MMU\n");
        return false;
    }

    load_boot(boot_path);
    load_rom(rom_path);

    return true;
}


void MMU::reset()
{
    booted = false;

    set(0x0100, 0x00);     // Reset MBC

    set(TIMA, 0x00);
    set(TMA, 0x00);
    set(TAC, 0x00);
    set(0xFF10, 0x80);     //<! NR10
    set(0xFF11, 0xBF);     //<! NR11
    set(0xFF12, 0xF3);     //<! NR12
    set(0xFF14, 0xBF);     //<! NR14
    set(0xFF16, 0x3F);     //<! NR21
    set(0xFF17, 0x00);     //<! NR22
    set(0xFF19, 0xBF);     //<! NR24
    set(0xFF1A, 0x7F);     //<! NR30
    set(0xFF1B, 0xFF);     //<! NR31
    set(0xFF1C, 0x9F);     //<! NR32
    set(0xFF1E, 0xBF);     //<! NR33
    set(0xFF20, 0xFF);     //<! NR41
    set(0xFF21, 0x00);     //<! NR42
    set(0xFF22, 0x00);     //<! NR43
    set(0xFF23, 0xBF);     //<! NR44
    set(0xFF24, 0x77);     //<! NR50
    set(0xFF25, 0xF3);     //<! NR51
    set(0xFF26, 0xF1);     //<! NR52
    set(LCDC, 0x91);
    set(SCY, 0x00);
    set(SCX, 0x00);
    set(LYC, 0x00);
    set(BGP, 0xFC);
    set(OBP0, 0xFF);
    set(OBP1, 0xFF);
    set(WY, 0x00);
    set(WX, 0x00);
    set(IE_ADDRESS, 0x00);

    update_ram();
}


/**
 * @brief      Get type of address controller at the specified address
 * Used to determine if we can read/write and map request
 * @param[in]  address  The address
 * @return     The address identity.
 */
address_type MMU::get_address_identity(uint16_t address)
{
    // Assume BOOT and ROM memroies both starts at 0x0000

    // boot ROM
    if (!booted && /*address >= BOOT_START &&*/ address <= BOOT_END) {
        return BOOT;
    }

    // Cartridge ROM
    if (address <= ROM0_END) {
        return ROM0;
    }
    else if (address <= ROM1_END) {
        return ROM1;
    }
    else if (address <= VRAM_END) {
        return VRAM;
    }
    else if (address <= SRAM_END) {
        return SRAM;
    }
    else if (address <= WRAM0_END) {
        return WRAM0;
    }
    else if (address <= WRAM1_END) {
        return WRAM1;
    }
    else if (address <= ECHO_END) {
        return ECHO;
    }
    else if (address <= OAM_END) {
        return OAM;
    }
    else if (address <= CRASH_END) {
        return CRASH;
    }
    else if (address <= IO_END) {
        return IO_RAM;
    }
    //else if (address <= HRAM_END) {
    return HRAM;
    //}
}


const char *MMU::display_address_identity(uint16_t address)
{
    address_type type = get_address_identity(address);

    switch(type) {
    case BOOT:   return "BOOT";
    case ROM0:   return "ROM0";
    case ROM1:   return "ROM1";
    case VRAM:   return "VRAM";
    case SRAM:   return "SRAM";
    case WRAM0:  return "WRA0";
    case WRAM1:  return "WRA1";
    case ECHO:   return "ECHO";
    case OAM:    return "OAM";
    case CRASH:  return "";
    case IO_RAM: return "I/O";
    case HRAM:   return "HRAM";
    }

    return "";
}


/**
 * @brief      Set the value at the desired address
 * @param[in]  address  The address
 * @param[in]  value    The value to write
 * @return     false if the address designed a READ only memory
 */
bool MMU::set(uint16_t address, uint8_t value)
{
    address_type identity = get_address_identity(address);

    // Cannot write over BOOT rom
    if (identity == BOOT) {
        return false;
    }

    // Write to ROM are passed to cartridge
    else if (identity == ROM0 || identity == ROM1) {
        cart->set(address, value);
        update_ram();
        return true;
    }

    // Write to External RAM are passed to cartridge
    else if (identity == SRAM && cart->has_ram()) {
        cart->set(address, value);
        update_ram();
    }

    // Writting to CRASH is ignored on DMG
    else if (identity == CRASH) {
        return false;
    }

    // ECHO memory
    else if (identity == ECHO) {
        address -= 0x2000;
    }

    // LCD_STATUS
    if (address == LCD_STATUS) {
        // Cannot overwrite on Mode and coincidence flag
        value &= 0b01111000;
    }

    // Sound Control
    // TODO: Check power on, if off, does not write! (except length)
    else if (address == NR52) {
        value &= 0xF0;
    }

    value = memory_masks(address, value);

    set_nocheck(address, value);

    handle_callbacks(address, value);

    return false;
}


/**
 * @brief      Same as set but does not triggers value modifications
 *
 * Cannot be used by the CPU or ROM code
 * @param[in]  address  The address
 * @param[in]  value    The value to write
 * @return     false if the address designed a READ only memory
 */
void MMU::set_nocheck(uint16_t address, uint8_t value)
{
    ram[address] = value;

    if (debugger != nullptr && debugger->breakpoint_activated) {
        debugger->feed_memory_write(address);
    }
}


uint8_t MMU::get(uint16_t address)
{
    address_type identity = get_address_identity(address);

    // ECHO memory
    if (identity == ECHO) {
        address -= 0x2000;
    }

    uint8_t value = get_nocheck(address);

    return memory_masks(address, value);
}


/**
 * @brief      Gets the value at given RAM address
 * Internal usage only! Cannot be used by CPU or ROM code
 * @param[in]  address  The address
 * @return     Value at that address
 */
uint8_t MMU::get_nocheck(uint16_t address)
{
    if (debugger != nullptr && debugger->breakpoint_activated) {
        debugger->feed_memory_read(address);
    }

    return ram[address];
}


/**
 * @brief      Apply mask on some values
 * Example: unused bits of IF should always read/set to 1
 */
uint8_t MMU::memory_masks(uint16_t address, uint8_t value)
{

    // LCD_STATUS
    if (address == LCD_STATUS) {
        // Last bit unused
        value |= 0b10000000;
    }

    // Joypad
    else if (address == JOYPAD) {
        // Last two bytes always 1
        value |= 0b11000000;
    }

    // Interrupts
    else if (address == IF_ADDRESS) {
        // Last three bit always set
        value |= 0xE0;
    }

    return value;
}


/**
 * @brief      Check function that should be called on RAM modification
 * @param[in]  address  The address
 * @param[in]  value    The value
 *
 * TODO: There are many errors here, no direct ram access should be allowed
 * except for debugger! Overload assignation and give a mutable object for CPU
 * instruction or something, many callback are'nt called as needed
 */
void MMU::handle_callbacks(uint16_t address, uint8_t value)
{
    // BOOT Status
    if (address == BOOT_ROM_ENABLE) {
        set_boot_rom_enable(value);
    }

    // LCD Control
    else if (address == LCDC) {
        ppu->set_lcdc(value);
    }

    // BG Palette
    else if (address == BGP) {
        ppu->set_bgp(value);
    } else if (address == OBP0) {
        ppu->set_obp(0, value);
    } else if (address == OBP1) {
        ppu->set_obp(1, value);
    }

    // Timer
    else if (address == DIV) {
        timer->set_DIV(value);
    } else if (address == TAC) {
        timer->set_TAC(value);
    } else if (address == TIMA) {
        timer->set_TIMA(value);
    }

    // OAM transfer
    else if (address == OAM_TRANSFER) {
        for (uint16_t i=0; i<OAM_SIZE; i++) {
            set_nocheck(OAM_START + i, get_nocheck((value * 0x0100) + i));
        }
    }

    // Sound - Channel 1
    else if (address == NR10) {
        apu->set_NR10(value);
    } else if (address == NR11) {
        apu->set_NR11(value);
    } else if (address == NR12) {
        apu->set_NR12(value);
    } else if (address == NR13) {
        apu->set_NR13(value);
    } else if (address == NR14) {
        apu->set_NR14(value);
    }

    // Sound - Channel 2
    else if (address == NR21) {
        apu->set_NR21(value);
    } else if (address == NR22) {
        apu->set_NR22(value);
    } else if (address == NR23) {
        apu->set_NR23(value);
    } else if (address == NR24) {
        apu->set_NR24(value);
    }

    // Sound - Channel 3
    else if (address == NR30) {
        apu->set_NR30(value);
    } else if (address == NR31) {
        apu->set_NR31(value);
    } else if (address == NR32) {
        apu->set_NR32(value);
    } else if (address == NR33) {
        apu->set_NR33(value);
    } else if (address == NR34) {
        apu->set_NR34(value);
    }

    // Sound - Channel 4
    else if (address == NR10) {
        apu->set_NR10(value);
    } else if (address == NR41) {
        apu->set_NR41(value);
    } else if (address == NR42) {
        apu->set_NR42(value);
    } else if (address == NR43) {
        apu->set_NR43(value);
    } else if (address == NR44) {
        apu->set_NR44(value);
    }

    // Sound Control
    else if (address == NR50) {
        apu->set_NR50(value);
    } else if (address == NR51) {
        apu->set_NR51(value);
    } else if (address == NR52) {
        apu->set_NR52(value);
    }
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


/**
 * @brief      Loads game
 * @param[in]  filepath  The filepath of the game rom
 * @return     true on success
 */
bool MMU::load_rom(std::string filepath)
{
    if (!cart->load(filepath)) {
        return false;
    }

    update_ram();

    return true;
}


/**
 * @brief      Loads boot ROM
 * @param[in]  path_rom     Where the ROM is
 * @return     { description_of_the_return_value }
 */
bool MMU::load_boot(std::string filepath)
{
    booted = false;

    info("Loading boot ROM: %s\n", filepath.c_str());

    if (filepath.size() <= 0) {
        info("No BOOT rom given\n");
        return false;
    }

    FILE *f = fopen(filepath.c_str(), "rb");
    if (f == NULL) {
        error("Unable to read provided boot ROM file\n");
        return false;
    }

    uint8_t byte;
    uint16_t dst = BOOT_START;
    while (fread(&byte, sizeof(uint8_t), 1, f) == 1) {
        boot[dst] = byte;
        ram[dst] = byte;

        dst++;
    }

    // Failure of reading not EOF related
    if (!feof(f)) {
        error("Error while reading provided boot ROM file\n");
        return false;
    }

    update_ram();

    return true;
}


/**
 * @brief      Loads arbitrary program
 */
bool MMU::load(uint8_t *program, size_t size, uint16_t dst)
{
    for (size_t i=0; i<size; i++) {
        ram[dst + i] = program[i];
    }

    return true;
}


/**
 * @brief      Shortcut for load
 */
bool MMU::load(uint8_t *program, size_t size)
{
    return load(program, size, 0x0000);
}


/**
 * @brief      Allows to control if BOOT is readable or not
 * @param[in]  value  The value 0x01 indicate boot is no longer needed
 */
void MMU::set_boot_rom_enable(uint8_t value)
{
    // Cannot restore boot once it is booted
    if (!booted) {
        set_booted(value & 0x01);
    }
}


/**
 * @brief      Set RAM content based on cartridge presence and BOOT status
 */
void MMU::update_ram()
{
    // Set ROM 0
    memcpy(ram, cart->mbc->memory, MBC_SIZE);

    // Set ROM N
    for (uint16_t i=MBC_SIZE; i<MBC_SIZE * 2; i++) {
        ram[i] = cart->get(i);
    }

    // Set SRAM
    for (uint16_t i=0; i<RAM_MBC_SIZE; i++) {
        ram[SRAM_START + i] = cart->get(SRAM_START + i);
    }

    // Put BOOT in RAM
    if (!booted) {
        memcpy(ram, boot, BOOT_SIZE);
    }
}


/**
 * @brief      Helper to set interrupts
 * @param[in]  interrupt_mask  The interrupt mask
 */
void MMU::trigger_interrupt(uint8_t interrupt_mask)
{
    set(IF_ADDRESS, ram[IF_ADDRESS] | interrupt_mask);
}


void MMU::set_booted(bool value)
{
    booted = value;

    update_ram();
}


bool MMU::is_booted()
{
    return booted;
}


void MMU::set_ppu(PPU *ppu)
{
    this->ppu = ppu;
}


void MMU::set_timer(Timer *timer)
{
    this->timer = timer;
}


void MMU::set_input(Input *input)
{
    this->input = input;
}


void MMU::set_apu(APU *apu)
{
    this->apu = apu;
}


void MMU::set_debugger(Debugger *debugger)
{
    this->debugger = debugger;
}


void MMU::serialize(std::ofstream &file)
{
    file.write(reinterpret_cast<char*>(&booted), sizeof(bool));

    file.write(reinterpret_cast<char*>(boot), sizeof(uint8_t) * BOOT_SIZE);
    file.write(reinterpret_cast<char*>(ram), sizeof(uint8_t) * RAM_SIZE);

    cart->serialize(file);
}


void MMU::deserialize(std::ifstream &file)
{
    file.read(reinterpret_cast<char*>(&booted), sizeof(bool));

    file.read(reinterpret_cast<char*>(boot), sizeof(uint8_t) * BOOT_SIZE);
    file.read(reinterpret_cast<char*>(ram), sizeof(uint8_t) * RAM_SIZE);

    cart->deserialize(file);
}
