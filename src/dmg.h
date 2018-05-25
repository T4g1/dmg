#ifndef DMG_H
#define DMG_H

#include <inttypes.h>

#define RAM_SIZE        0xFFFF
#define REGISTER_COUNT  8
#define R16_COUNT       3

// 8-Bits registers
#define A               0   //<! Accumulator
#define F               1   //<! Flags (Z N H C - - - -)
#define B               2
#define C               3
#define D               4
#define E               5
#define H               6
#define L               7

// 16-Bits registers
#define BC              0
#define DE              1
#define HL              2

// Flags
#define FZ               7  //<! Zero Flag
#define FN               6  //<! Add/Sub Flag
#define FH               5  //<! Half Carry Flag
#define FC               4  //<! Carry Flag

struct dmg_state {
    int clock;

    uint8_t ram[RAM_SIZE];
    uint8_t reg[REGISTER_COUNT];
    uint16_t *r16[R16_COUNT];       //<! Shortcut for 16bits registers
    uint16_t SP;                    //<! Stack Pointer
    uint16_t PC;                    //<! Program Counter
};

#endif /* DMG_H */
