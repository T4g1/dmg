#ifndef TIMER_H
#define TIMER_H

#include <stdlib.h>
#include <stdint.h>

// Timer Control masks
#define TIMER_START         0b00000100
#define INPUT_CLOCK         0b00000011

class MMU;


/**
 * @brief      Handle the DMG main timer
 */
class Timer {
public:
    Timer(MMU *mmu);

    void update(size_t clock_cycles);

    void set_DIV(uint8_t value);
    void set_TAC(uint8_t value);

private:
    MMU *mmu;

    bool enabled;
    size_t clock_select;
};

#endif /* TIMER_H */
