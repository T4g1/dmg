#include <inttypes.h>

#define MAX_OPCODES         255

void nop(uint16_t *PC, int *clock, uint8_t *reg, uint16_t **r16, uint8_t *ram);

typedef void (*cpu_callback)(uint16_t*, int*, uint8_t*, uint16_t**, uint8_t*);
