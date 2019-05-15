#ifndef CPU_H
#define CPU_H

#include <inttypes.h>
#include <stdbool.h>
#include <cstddef>

#include "mmu.h"
#include "gui/cpu_gui.h"

#define MAX_OPCODES         256
#define REGISTER_COUNT  10  //<! 8x8-bit for standard registers + 2x8-bit for SP (16-bit)

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
#define SP              8   //<! Stack Pointer

// Flags
#define FZ               7  //<! Zero Flag
#define FN               6  //<! Add/Sub Flag
#define FH               5  //<! Half Carry Flag
#define FC               4  //<! Carry Flag

class CPU;
typedef void (CPU::*cpu_callback)(void);

/**
 * @brief      Central Processing Unit
 */
class CPU {
public:
    CPU(MMU *mmu);

    void reset();
    bool step();

    size_t clock;

    uint8_t reg[REGISTER_COUNT];
    uint16_t PC;                    //<! Program Counter

    bool get_flag(size_t flag);

    uint16_t reg16(size_t i);

    void display_registers();

private:
    MMU *mmu;

    bool IME;       // Interrupts are enabled when this is at true

    cpu_callback l_callback[MAX_OPCODES];

    void ld8_mmu(uint16_t address, const uint8_t *src, size_t size, size_t clock);

    void ld8(uint8_t *dst, const uint8_t *src, size_t size, size_t clock);
    void ld16(uint8_t *dst, const uint8_t *src, size_t size, size_t clock);

    void inc8(uint8_t *address);
    void dec8(uint8_t *address);

    void add8();
    void add16(uint8_t *dst, const uint8_t *src);

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
    void or_xor_and_cp();
    void sub();
    void jp();
    void jp_hl();
    void ei();
    void di();
    void rst();

    friend class CPUGui;
};

#endif /* CPU_H */
