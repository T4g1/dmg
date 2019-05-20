#include "timer.h"

#include "mmu.h"


Timer::Timer(MMU *mmu) : mmu(mmu)
{
    enabled = false;
    clock_select = 0;
}


/**
 * @brief      Update the timer given an increment of clock cycle
 * @param[in]  clock_cycles  CPU clock cycles added since last update
 */
void Timer::update(size_t /*clock_cycles*/)
{
    // TODO
}


/**
 * @brief      DIV value gets changed
 * @param[in]  value  The new value
 */
void Timer::set_DIV(uint8_t /*value*/)
{
    mmu->set(DIV, 0x00);
}


/**
 * @brief      TAC value gets changed:
 * @param[in]  value  The new value
 */
void Timer::set_TAC(uint8_t value)
{
    enabled = value & TIMER_START;

    clock_select = value & INPUT_CLOCK;
}
