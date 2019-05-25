#include "timer.h"

#include "mmu.h"
#include "cpu.h"
#include "log.h"


Timer::Timer() : mmu(nullptr)
{

}


bool Timer::init()
{
    if (mmu == nullptr) {
        error("No MMU linked with Timer\n");
        return false;
    }

    reset();

    return true;
}


void Timer::reset()
{
    clock = 0;

    enabled = false;
    clock_select = 0;

    last_increment = 0;
    last_div_increment = 0;
}


/**
 * @brief      Update the timer
 */
void Timer::step()
{
    const size_t frequencies[] = {
        1024,
        16,
        64,
        256
    };

    clock += 16;

    if (clock - last_div_increment > DIV_FREQUENCY) {
        mmu->set_nocheck(DIV, mmu->get(DIV) + 1);
        last_div_increment += DIV_FREQUENCY;
    }

    if (clock - last_div_increment > frequencies[clock_select]) {
        mmu->set(TIMA, mmu->get(TIMA) + 1);
        last_div_increment += frequencies[clock_select];
    }
}


/**
 * @brief      DIV value gets changed
 * @param[in]  value  The new value
 */
void Timer::set_DIV(uint8_t /*value*/)
{
    mmu->set_nocheck(DIV, 0x00);
}


/**
 * @brief      TAC value gets changed:
 * @param[in]  value  The new value
 */
void Timer::set_TAC(uint8_t value)
{
    bool new_enabled = value & TIMER_START;

    size_t new_clock_select = value & INPUT_CLOCK;

    // Has been enabled
    if ((new_enabled && !enabled) ||
    // Change cycle
        (new_clock_select != clock_select)) {
        last_increment = clock;
    }

    enabled = new_enabled;
    clock_select = new_clock_select;
}


void Timer::set_TIMA(uint8_t value)
{
    // Overflow
    if (value == 0x00) {
        mmu->set_nocheck(TIMA, mmu->get(TMA));

        // Interrupt
        mmu->trigger_interrupt(INT_TIMER_MASK);
    }
}


void Timer::set_mmu(MMU *mmu)
{
    this->mmu = mmu;
}


void Timer::serialize(std::ofstream &file)
{
    file.write(reinterpret_cast<char*>(&enabled), sizeof(bool));

    file.write(reinterpret_cast<char*>(&clock_select), sizeof(size_t));
    file.write(reinterpret_cast<char*>(&last_increment), sizeof(size_t));
    file.write(reinterpret_cast<char*>(&last_div_increment), sizeof(size_t));
    file.write(reinterpret_cast<char*>(&clock), sizeof(size_t));
}


void Timer::deserialize(std::ifstream &file)
{
    file.read(reinterpret_cast<char*>(&enabled), sizeof(bool));

    file.read(reinterpret_cast<char*>(&clock_select), sizeof(size_t));
    file.read(reinterpret_cast<char*>(&last_increment), sizeof(size_t));
    file.read(reinterpret_cast<char*>(&last_div_increment), sizeof(size_t));
    file.read(reinterpret_cast<char*>(&clock), sizeof(size_t));
}
