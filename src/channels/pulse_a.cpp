#include "pulse_a.h"


PulseA::PulseA()
{

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
    duty_frequency = 0;

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
    mmu->set_nocheck(NR52, mmu->get(NR52) | SOUND_PULSE_A_ON_FLAG);

    ve_timer = SOUND_VOLUME_ENVELOPE_FREQ;
    ve_enabled = true;

    length = 64;                    // Reload length
    set_NR12(mmu->get(NR12));       // Reload volume

    // TODO: duty_clock = now?

    sweep_time_actual = sweep_time;
    sweep_flag = false;
}


/**
 * @brief      Handle Pulse A frequency sweep
 */
void PulseA::frequency_sweep()
{
    size_t new_frequency;

    sweep_time_actual -= 1;

    if (sweep_time_actual > 0) {
        return;
    }

    shadow_frequency = duty_frequency;    // Copy frequency to shadow
    sweep_time_actual = sweep_time;       // Reset sweep time

    if (sweep_flag && sweep_time != 0) {
        new_frequency = compute_frequency(shadow_frequency, sweep_shift, sweep_decrease);
        if (new_frequency >= 2048) {
            enabled = false;
        } else if (sweep_shift != 0) {
            shadow_frequency = new_frequency;
            duty_frequency = new_frequency;
            // TODO: Update NR13, NR14
        }
    }

    // Set internal enabled flag
    sweep_flag = sweep_shift != 0 || sweep_time != 0;

    if (sweep_shift == 0) {
        return;
    }

    // Compute new frequency
    new_frequency = compute_frequency(shadow_frequency, sweep_shift, sweep_decrease);
    if (new_frequency >= 2048) {
        enabled = false;
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
    length = value & 0b00111111;
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

    dac_enabled = volume != 0;
}


/**
 * @brief      Frequency lo
 * @param[in]  value  The value
 */
void PulseA::set_NR13(uint8_t value)
{
    duty_frequency = (duty_frequency & 0x0700) + value;
}


/**
 * @brief      Restart, stop on end of length and frequency hi
 * @param[in]  value  The value
 */
void PulseA::set_NR14(uint8_t value)
{
    restart = value & 0b10000000;
    length_limitation = value & 0b01000000;

    uint16_t frequency_hi = (value & 0b00000111) << 8;
    uint16_t frequency_lo = (duty_frequency & 0x00FF);
    duty_frequency = frequency_hi + frequency_lo;
}


/**
 * @brief      Pulse A frequency
 * @return     The PA frequency.in Hz
 */
size_t PulseA::get_frequency()
{
    return (2048 - duty_frequency) * 4;
}


/**
 * @brief      Compute sweep new frequency
 * @param[in]  shadow    The frequency value before
 * @param[in]  shift     The shift to operate
 * @param[in]  decrease  Decrease or increase frequency
 */
size_t PulseA::compute_frequency(size_t shadow, size_t shift, bool decrease)
{
    int factor = 1;
    if (decrease) {
        factor = -1;
    }

    return shadow + (factor * (shadow >> shift));
}

