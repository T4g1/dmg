#ifndef CPU_H
#define CPU_H

#include <inttypes.h>
#include <stdbool.h>
#include <cstddef>

#include "mmu.h"
#include "gui/debugger.h"

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
typedef void (CPU::*cpu_callback)(uint8_t opcode);

/**
 * @brief      Central Processing Unit
 */
class CPU {
public:
    CPU(MMU *mmu);

    void reset();
    bool step();

    size_t clock;

    // TODO: Force F flag to be always 0-3 at 0
    uint8_t reg[REGISTER_COUNT];
    uint16_t PC;                    //<! Program Counter

    void set_flag(size_t flag, bool value);
    bool get_flag(size_t flag);

    uint16_t reg16(size_t i);

private:
    MMU *mmu;

    bool IME;       // Interrupts are enabled when this is at true

    cpu_callback l_callback[MAX_OPCODES];

    uint8_t *get_target(size_t index);
    uint8_t *get_target16(size_t index);

    void ld8_mmu(uint16_t address, const uint8_t *src, size_t size, size_t clock);

    void ld8(uint8_t *dst, const uint8_t *src, size_t size, size_t clock);
    void ld16(uint8_t *dst, const uint8_t *src, size_t size, size_t clock);

    void inc8(uint8_t *address);
    void dec8(uint8_t *address);

    void add8(uint8_t opcode);
    void add16(uint8_t *dst, const uint8_t *src);
    void addr8(uint8_t *dst, int value);

    void prefix_CB(uint8_t opcode);
    void CB_set(uint8_t opcode);

    void push(uint8_t opcode);
    void pop(uint8_t opcode);
    void call(uint8_t opcode);
    void ret(uint8_t opcode);
    void cpl(uint8_t opcode);
    void ccf(uint8_t opcode);
    void daa(uint8_t opcode);
    void scf(uint8_t opcode);
    void stop(uint8_t opcode);
    void halt(uint8_t opcode);
    void add(uint8_t opcode);
    void inc(uint8_t opcode);
    void dec(uint8_t opcode);
    void jr(uint8_t opcode);
    void ld(uint8_t opcode);
    void rxa(uint8_t opcode);
    void nop(uint8_t opcode);
    void or_xor_and_cp(uint8_t opcode);
    void sub(uint8_t opcode);
    void jp(uint8_t opcode);
    void jp_hl(uint8_t opcode);
    void ei(uint8_t opcode);
    void di(uint8_t opcode);
    void rst(uint8_t opcode);

    friend class Debugger;
};

#endif /* CPU_H */
