#include "pulse_b.h"


PulseB::PulseB()
{
    enabled_flag = SOUND_PULSE_B_ON_FLAG;
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
    mmu->set_nocheck(NR52, mmu->get(NR52) | enabled_flag);

    ve_timer = SOUND_VOLUME_ENVELOPE_FREQ;

    // Reload length
    if (length == 0) {
        length = 64;
        if (sequencer_step & 0x01) {
            length_counter();
        }
    }
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


