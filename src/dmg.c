#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>

#define RAM_SIZE        0xFFFF
#define REGISTER_COUNT  8
#define R16_COUNT       3

#define A               0   //<! Accumulator
#define F               1   //<! Flags (Z N H C - - - -)
#define B               2
#define C               3
#define D               4
#define E               5
#define H               6
#define L               7

#define BC              0
#define DE              1
#define HL              1

uint8_t ram[RAM_SIZE];
uint8_t reg[REGISTER_COUNT];
uint16_t *r16[R16_COUNT];       //<! Shortcut for 16bits registers
uint16_t SP;                    //<! Stack Pointer
uint16_t PC;                    //<! Program Counter

void init()
{
    r16[BC] = (uint16_t*) &reg[B];
    r16[DE] = (uint16_t*) &reg[D];
    r16[HL] = (uint16_t*) &reg[H];
}

void reset()
{
    reg[A] = 0x01;
    reg[F] = 0xB0;
    reg[B] = 0x00;
    reg[C] = 0x13;
    reg[D] = 0x00;
    reg[E] = 0xD8;
    reg[H] = 0x01;
    reg[L] = 0x4D;

    SP = 0xFFFE;

    ram[0xFF05] = 0x00;     //<! TIMA
    ram[0xFF06] = 0x00;     //<! TMA
    ram[0xFF07] = 0x00;     //<! TAC
    ram[0xFF10] = 0x80;     //<! NR10
    ram[0xFF11] = 0xBF;     //<! NR11
    ram[0xFF12] = 0xF3;     //<! NR12
    ram[0xFF14] = 0xBF;     //<! NR14
    ram[0xFF16] = 0x3F;     //<! NR21
    ram[0xFF17] = 0x00;     //<! NR22
    ram[0xFF19] = 0xBF;     //<! NR24
    ram[0xFF1A] = 0x7F;     //<! NR30
    ram[0xFF1B] = 0xFF;     //<! NR31
    ram[0xFF1C] = 0x9F;     //<! NR32
    ram[0xFF1E] = 0xBF;     //<! NR33
    ram[0xFF20] = 0xFF;     //<! NR41
    ram[0xFF21] = 0x00;     //<! NR42
    ram[0xFF22] = 0x00;     //<! NR43
    ram[0xFF23] = 0xBF;     //<! NR44
    ram[0xFF23] = 0x77;     //<! NR50
    ram[0xFF23] = 0xF3;     //<! NR51
    ram[0xFF23] = 0xF1;     //<! NR52
    ram[0xFF23] = 0x91;     //<! LCDC
    ram[0xFF23] = 0x00;     //<! SCY
    ram[0xFF23] = 0x00;     //<! SCX
    ram[0xFF23] = 0x00;     //<! LYC
    ram[0xFF23] = 0xFC;     //<! BGP
    ram[0xFF23] = 0xFF;     //<! OBP0
    ram[0xFF23] = 0xFF;     //<! OBP1
    ram[0xFF23] = 0x00;     //<! WY
    ram[0xFF23] = 0x00;     //<! WX
    ram[0xFF23] = 0x00;     //<! IE
}

bool load(const char *filepath, uint16_t dst)
{
    FILE *f = fopen(filepath, "rb");
    if (f == NULL) {
        fprintf(stderr, "Unable to read provided boot ROM file\n");
        return false;
    }

    uint8_t byte;
    while (fread(&byte, sizeof(uint8_t), 1, f) == 1) {
        ram[dst++] = byte;
    }

    // Failure of reading not EOF related
    if (!feof(f)) {
        fprintf(stderr, "Error while reading provided boot ROM file\n");
        return false;
    }

    return true;
}

int main(int argc, char *argv[])
{
    if (argc != 3) {
        fprintf(stderr, "Usage: dmg <BOOT ROM> <GAME ROM>\n");
        return EXIT_FAILURE;
    }

    char *boot_rom = argv[1];
    char *game_rom = argv[2];

    init();
    reset();

    bool running = true;

    fprintf(stdout, "Loading boot ROM from %s\n", boot_rom);
    running &= load(boot_rom, 0x0000);

    //fprintf("Loading game ROM from %s\n", game_rom);

    while (running) {
        fprintf(stdout, "Nintendo(r) but no cartridge so big black rectangle(r) instead\n");
        fprintf(stdout, "*jingles*\n");
        fprintf(stdout, "Thanks for playing\n");

        running = false;
    }

    return EXIT_SUCCESS;
}
