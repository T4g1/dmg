#ifndef CPU_H
#define CPU_H

#include <inttypes.h>
#include <stdbool.h>

#include "mmu.h"

#define MAX_OPCODES         255
#define REGISTER_COUNT  8

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
#define AF              A
#define BC              B
#define DE              D
#define HL              H

// Flags
#define FZ               7  //<! Zero Flag
#define FN               6  //<! Add/Sub Flag
#define FH               5  //<! Half Carry Flag
#define FC               4  //<! Carry Flag

class CPU;
typedef void (CPU::*cpu_callback)(void);

class CPU {
public:
    CPU(MMU *mmu);

    void reset();
    void step();

    size_t clock;

    uint8_t reg[REGISTER_COUNT];
    uint16_t SP;                    //<! Stack Pointer
    uint16_t PC;                    //<! Program Counter

    uint16_t reg16(size_t i);

private:
    MMU *mmu;

    cpu_callback l_callback[MAX_OPCODES];

    void ld8(void *dst, void *src, size_t size, size_t clock);
    void ld16(uint8_t *dst, uint8_t *src, size_t size, size_t clock);

    void inc8(uint8_t *address);
    void dec8(uint8_t *address);

    void add16(uint8_t *dst, uint8_t *src);

    void prefix_CB();
    void CB_set();

    void push();
    void pop();
    void call();
    void ret();
    void cpl();
    void ccf();
    void daa();
    void scf();
    void stop();
    void halt();
    void add();
    void inc();
    void dec();
    void jr();
    void ld();
    void rxa();
    void nop();
    void _xor();
};

#endif /* CPU_H */
