#include "mmu.h"

#include <stdio.h>
#include <string.h>

#include "utils.h"
#include "log.h"
#include "ppu.h"
#include "timer.h"


MMU::MMU() : cart(nullptr), ppu(nullptr), timer(nullptr)
{

}


bool MMU::init(const char *path_bios, Cartridge *cartridge)
{
    if (ppu == nullptr) {
        error("No PPU linked with MMU\n");
        return false;
    }

    if (timer == nullptr) {
        error("No Timer linked with MMU\n");
        return false;
    }

    if (path_bios != nullptr) {
        if (!load(path_bios)) {
            return false;
        }
    }

    booted = false;

    set_cartridge(cartridge);

    reset();

    return true;
}


void MMU::reset()
{
    booted = false;

    set(0x0100, 0x00);     // Reset MBC

    set(0xFF05, 0x00);     //<! TIMA
    set(0xFF06, 0x00);     //<! TMA
    set(0xFF07, 0x00);     //<! TAC
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
    set(LCDC, 0x91);       //<! LCDC
    set(SCY, 0x00);        //<! SCY
    set(SCX, 0x00);        //<! SCX
    set(LYC, 0x00);        //<! LYC
    set(0xFF47, 0xFC);     //<! BGP
    set(0xFF48, 0xFF);     //<! OBP0
    set(0xFF49, 0xFF);     //<! OBP1
    set(0xFF4A, 0x00);     //<! WY
    set(0xFF4B, 0x00);     //<! WX
    set(0xFFFF, 0x00);     //<! IE

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
#ifdef DEBUG
    if (ppu == nullptr) {
        error("PPU is not linked to the MMU: Segfault will happen. Use mmu->set_ppu()\n");
    }
#endif
    address_type identity = get_address_identity(address);
    //debug("set 0x%04X 0x%02X\n", address, value);

    // Cannot write over BOOT rom
    if (identity == BOOT) {
        return false;
    }

    // Write to ROM are passed to cartridge
    else if (identity == ROM0 || identity == ROM1) {
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

    ram[address] = value;

    handle_callbacks(address, value);

    return false;
}


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
    }

    // Timer
    else if (address == DIV) {
        timer->set_DIV(value);
    }
    else if (address == TAC) {
        timer->set_TAC(value);
    }
    else if (address == TIMA) {
        timer->set_TIMA(value);
    }

    // OAM transfer
    else if (address == OAM_TRANSFER) {
        // TODO: Takes more time?
        memcpy(ram + OAM_START, ram + (value * 0x0100), OAM_SIZE);
    }

    // Joypad
    else if (address == JOYPAD) {
        // Last two bytes always 1
        ram[address] |= 0b11000000;
    }

    // Interrupts
    else if (address == IF_ADDRESS) {
        // IF cannot triggers if IE corresponding bit is not set
        ram[address] &= ram[IE_ADDRESS];

        // First three bit always set
        ram[address] |= 0xE0;
    }
}


/**
 * @brief      Give the emulated address of the given DMG address
 * @param[in]  address  The address in the DMG memory
 * @return     Pointer to the immutable value
 */
const void *MMU::at(uint16_t address)
{
    address_type identity = get_address_identity(address);

    // ECHO memory
    if (identity == ECHO) {
        address -= 0x2000;
    }

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


/**
 * @brief      Loads boot ROM
 * @param[in]  path_rom     Where the ROM is
 * @return     { description_of_the_return_value }
 */
bool MMU::load(const char *path_rom)
{
    info("Loading boot ROM: %s\n", path_rom);

    FILE *f = fopen(path_rom, "rb");
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
 * @brief      Plugs a cartridge inside the memory
 * @param      cart  Cartridge to be used
 */
void MMU::set_cartridge(Cartridge *cart)
{
    this->cart = cart;

    update_ram();
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
    // Put ROM in RAM
    if (cart) {
        memcpy(ram, cart->mbc->memory, MBC_SIZE);

        for (uint16_t i=MBC_SIZE; i<MBC_SIZE * 2; i++) {
            ram[i] = *(uint8_t*) cart->at(i);
        }
    }

    // Put BOOT in RAM
    if (!booted) {
        memcpy(ram, boot, BOOT_SIZE);
    }
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
