#include "wave.h"


Wave::Wave()
{

}


Wave::~Wave()
{

}


bool Wave::init()
{
    Channel::init();

    volume_shift = 0;

    // Wave
    wave_position = 0;
    wave_clock = 0;
    wave_frequency = 0;

    return true;
}


/**
 * @brief      Produce wave output
 */
void Wave::process()
{
    wave_clock += get_frequency();

    // Mute Wave
    if (!enabled) {
        output = 0;
    } else {
        // Generate the output
        uint16_t address = SOUND_WAVE_REG_START + (wave_position >> 1);
        uint8_t value = mmu->get(address);
        if (wave_position & 0x01) {
            output = value & 0x0F;
        } else {
            output = value >> SOUND_WAVE_REG_SIZE;
        }

        output = output >> volume_shift;
    }

    // Step the wave position
    wave_position = (wave_position + 1) % SOUND_WAVE_REG_COUNT;
}


/**
 * @brief      On restart
 */
void Wave::trigger()
{
    enabled = true;
    mmu->set_nocheck(NR52, mmu->get(NR52) | SOUND_WAVE_ON_FLAG);

    ve_timer = SOUND_VOLUME_ENVELOPE_FREQ;

    length = 256;                   // Reload length
    set_NR32(mmu->get(NR32));       // Reload volume

    wave_position = 0;

    // TODO: wave_clock = now?
}


/**
 * @brief      Set channel 3 - wave on/off
 * @param[in]  value  The value
 */
void Wave::set_NR30(uint8_t value)
{
    enabled = value & 0x80;
}


/**
 * @brief      Set duration of the wave
 * @param[in]  value Duration of the wave
 */
void Wave::set_NR31(uint8_t value)
{
    length = value;
}


/**
 * @brief      Set level of output as a shift count
 * Wave data is on 4 bits so shift 4 times give no sound.
 * @param[in]  value  Output level
 */
void Wave::set_NR32(uint8_t value)
{
    volume_shift = (value & 0x60) >> 5;
    if (volume_shift == 0) {
        dac_enabled = false;
        volume_shift = 4;
    } else {
        dac_enabled = true;
        volume_shift -= 1;
    }
}


/**
 * @brief      Lower 8bits of the frequency
 * @param[in]  value  Value of those lower 8 bits
 */
void Wave::set_NR33(uint8_t value)
{
    wave_frequency = (wave_frequency & 0x0700) + value;
}


/**
 * @brief      Set wave frequency hi and control wave flags
 * @param[in]  value Value of that
 */
void Wave::set_NR34(uint8_t value)
{
    restart = value & 0b10000000;
    length_limitation = value & 0b01000000;

    uint16_t frequency_hi = (value & 0b00000111) << 8;
    uint16_t frequency_lo = (wave_frequency & 0x00FF);
    wave_frequency = frequency_hi + frequency_lo;
}


/**
 * @brief      Wave frequency needs conversion to be in Hz
 * @return     The wave frequency.in Hz
 */
size_t Wave::get_frequency()
{
    return (2048 - wave_frequency) * 2;
}
