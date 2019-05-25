#ifndef TIMER_H
#define TIMER_H

#include <stdlib.h>
#include <stdint.h>

#include "defines.h"


class MMU;


/**
 * @brief      Handle the DMG main timer
 */
class Timer {
private:
    MMU *mmu;

    bool enabled;
    size_t clock_select;

    size_t last_increment;
    size_t last_div_increment;

public:
    size_t clock;

    Timer();

    bool init();
    void reset();
    void step();

    void set_DIV(uint8_t value);
    void set_TAC(uint8_t value);
    void set_TIMA(uint8_t value);

    void set_mmu(MMU *mmu);
};

#endif /* TIMER_H */
