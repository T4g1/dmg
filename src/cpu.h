#ifndef CPU_H
#define CPU_H

#include "dmg.h"

#define MAX_OPCODES         255

void ld(struct dmg_state *ds, uint8_t opcode);
void nop(struct dmg_state *ds, uint8_t opcode);
void xor(struct dmg_state *ds, uint8_t opcode);

typedef void (*cpu_callback)(struct dmg_state *ds, uint8_t opcode);

#endif /* CPU_H */
