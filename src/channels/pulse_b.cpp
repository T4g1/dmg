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

    ve_timer = ve_period;
    ve_enabled = true;
    volume = ve_volume;

    reload_length(64);
    set_NR22(mmu->get(NR22));       // Reload volume

    // TODO: duty_clock = now?
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
    uint8_t old_ve_period = ve_period;
    uint8_t old_ve_add = ve_add;

    ve_volume = (value & 0b11110000) >> 4;
    ve_add = value & 0b00001000;
    ve_period = value & 0b00000111;
    ve_timer = ve_period;

    // Zombie mode ? This fixes Zelda intro for pulse B but it doe snot emulate all cases
    if (old_ve_period == 0 && ve_period == 0) {
        if (old_ve_add == ve_add) {
            // Addition
            if (ve_add) {
                /*if (volume < 0x0F) {
                    volume += 1;
                }*/
            }
            // Substraction
            else {
                if (volume > 0x00) {
                    volume -= 1;
                }
            }
        }
    }

    dac_enabled = value & 0b11111000;
    if (!dac_enabled) {
        disable_dac();
    }
}


void PulseB::set_NR23(uint8_t value)
{
    PulseA::set_NR13(value);
}


void PulseB::set_NR24(uint8_t value)
{
    PulseA::set_NR14(value);
}


