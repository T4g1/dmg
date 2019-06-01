#include "pulse_a.h"


PulseA::PulseA()
{
    enabled_flag = SOUND_PULSE_A_ON_FLAG;
}


PulseA::~PulseA()
{

}


bool PulseA::init()
{
    Channel::init();

    // Wave duty
    duty = 0;
    duty_clock = 0;
    duty_position = 0;

    // Sweep Frequency
    sweep_time = 0;
    sweep_decrease = false;
    sweep_shift = 0;
    sweep_time_actual = 0;
    shadow_frequency = 0;
    sweep_flag = false;

    return true;
}


/**
 * @brief      Produce pulse A output
 */
void PulseA::process()
{
    // Duty look-up table
    const bool dutys[SOUND_PULSE_A_DUTY_COUNT][SOUND_PULSE_A_DUTY_SIZE] = {
        {0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 0, 0, 0, 1, 1, 1},
        {0, 1, 1, 1, 1, 1, 1, 0}
    };

    duty_clock += get_frequency();

    output = dutys[duty][duty_position] * volume;

    // Step the duty position
    duty_position = (duty_position + 1) % SOUND_PULSE_A_DUTY_SIZE;
}


/**
 * @brief      On restart
 */
void PulseA::trigger()
{
    enabled = true;
    mmu->set_nocheck(NR52, mmu->get(NR52) | enabled_flag);

    ve_timer = SOUND_VOLUME_ENVELOPE_FREQ;
    ve_enabled = true;

    // Reload length
    if (length == 0) {
        length = 64;
        if (sequencer_step & 0x01) {
            length_counter();
        }
    }
    set_NR12(mmu->get(NR12));       // Reload volume

    // TODO: duty_clock = now?

    shadow_frequency = frequency_raw;       // Copy frequency to shadow
    sweep_time_actual = sweep_time;         // Reset sweep time
    sweep_flag = sweep_shift != 0 || sweep_time != 0;
    if (sweep_shift != 0 && compute_frequency() >= 2048) {
        disable();
    }
}


/**
 * @brief      Handle Pulse A frequency sweep
 */
void PulseA::frequency_sweep()
{
    if (sweep_time_actual > 0) {
        sweep_time_actual -= 1;
    }

    if (sweep_time_actual > 0) {
        return;
    }

    sweep_time_actual = sweep_time;         // Reset sweep time

    if (!sweep_flag || sweep_time == 0) {
        return;
    }

    size_t new_frequency = compute_frequency();
    if (new_frequency >= 2048) {
        disable();
    } else if (sweep_shift != 0) {
        shadow_frequency = new_frequency;
        frequency_raw = new_frequency;
        // TODO: Update NR13, NR14

        if (compute_frequency() >= 2048) {
            disable();
        }
    }
}


/**
 * @brief      Reads sweep related registers
 * @param[in]  value  The value
 */
void PulseA::set_NR10(uint8_t value)
{
    sweep_time = (value & 0b01110000) >> 4;
    sweep_decrease = value & 0b00001000;
    sweep_shift = value & 0b00000111;
}


/**
 * @brief      Wave duty and length
 * @param[in]  value  The value
 */
void PulseA::set_NR11(uint8_t value)
{
    duty = (value & 0b11000000) >> 6;
    length = 64 - (value & 0b00111111);
}


/**
 * @brief      Volume
 * @param[in]  value  The value
 */
void PulseA::set_NR12(uint8_t value)
{
    volume = (value & 0b11110000) >> 4;
    ve_add = value & 0b00001000;
    ve_period = value & 0b00000111;

    dac_enabled = value & 0b11111000;
    if (!dac_enabled) {
        disable_dac();
    }
}


/**
 * @brief      Frequency lo
 * @param[in]  value  The value
 */
void PulseA::set_NR13(uint8_t value)
{
    set_NRX3(value);
}


/**
 * @brief      Restart, stop on end of length and frequency hi
 * @param[in]  value  The value
 */
void PulseA::set_NR14(uint8_t value)
{
    set_NRX4(value);
}


/**
 * @brief      Pulse A frequency
 * @return     The PA frequency.in Hz
 */
size_t PulseA::get_frequency()
{
    return (2048 - frequency_raw) * 4;
}


/**
 * @brief      Compute sweep new frequency
 */
size_t PulseA::compute_frequency()
{
    int factor = 1;
    if (sweep_decrease) {
        factor = -1;
    }

    return shadow_frequency + (factor * (shadow_frequency >> sweep_shift));
}

