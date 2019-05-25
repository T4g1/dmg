#ifndef CPU_H
#define CPU_H

#include <stdbool.h>
#include <cstddef>
#include <iostream>
#include <fstream>

#include "defines.h"
#include "mmu.h"
#include "gui/debugger.h"

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

class CPU;
typedef void (CPU::*cpu_callback)(uint8_t opcode);

/**
 * @brief      Central Processing Unit
 */
class CPU {
private:
    MMU *mmu;

    bool IME;           // Interrupts calls are enabled when this is at true
    bool halted;        // Waits for an interuption or not
    bool halt_aborted;  // Tried to halt but was not able to (TODO)
    bool halt_bug;      // Bug with halt triggerred or not

    bool ei_requested;  // Enabe interrupts pending?
    size_t ei_delay;    // How many step before acknowledging ei request

    cpu_callback l_callback[MAX_OPCODES];

    uint8_t *get_target(size_t index);
    uint8_t get_target_value(size_t index);
    uint8_t *get_target16(size_t index);


    void ld8(uint8_t *dst, uint8_t src, size_t size, size_t ticks);
    void ld8_mmu(uint16_t address, uint8_t src, size_t size, size_t ticks);
    void ld16(uint8_t *dst, uint8_t src_l, uint8_t src_h, size_t size, size_t ticks);
    void ld16_mmu(uint16_t address, uint16_t src, size_t size, size_t ticks);

    void inc8(uint8_t *address);
    void inc8_mmu(uint16_t address);
    void dec8(uint8_t *address);
    void dec8_mmu(uint16_t address);

    void add8(uint8_t opcode);
    void add16(uint8_t *dst, uint8_t *src);
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
    void ei_di(uint8_t opcode);
    void rst(uint8_t opcode);

    void _call(uint16_t address);
    bool handle_interrupts();

    friend class Debugger;

public:
    size_t clock;

    // TODO: Force F flag to be always 0-3 at 0
    uint8_t reg[REGISTER_COUNT];
    uint16_t PC;                    //<! Program Counter

    CPU();

    bool init();
    void reset();
    bool step();

    void set_flag(size_t flag, bool value);
    bool get_flag(size_t flag);

    uint16_t reg16(size_t i);

    void set_mmu(MMU *mmu);

    void serialize(std::ofstream &file);
    void deserialize(std::ifstream &file);
};

#endif /* CPU_H */
