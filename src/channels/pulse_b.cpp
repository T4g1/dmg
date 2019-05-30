#include "pulse_b.h"


PulseB::PulseB()
{

}


PulseB::~PulseB()
{

}


/**
 * @brief      On restart
 */
void PulseB::trigger()
{
    enabled = true;
    mmu->set_nocheck(NR52, mmu->get(NR52) | SOUND_PULSE_A_ON_FLAG);

    ve_timer = SOUND_VOLUME_ENVELOPE_FREQ;

    length = 64;                    // Reload length
    set_NR22(mmu->get(NR22));       // Reload volume
}


void PulseB::frequency_sweep()
{
    // Does nothing
}


void PulseB::set_NR21(uint8_t value)
{
    PulseA::set_NR11(value);
}


void PulseB::set_NR22(uint8_t value)
{
    PulseA::set_NR12(value);
}


void PulseB::set_NR23(uint8_t value)
{
    PulseA::set_NR13(value);
}


void PulseB::set_NR24(uint8_t value)
{
    PulseA::set_NR14(value);
}


