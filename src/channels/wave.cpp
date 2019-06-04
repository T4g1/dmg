#include "wave.h"

#include "../dmg.h"


Wave::Wave()
{
    enabled_flag = SOUND_WAVE_ON_FLAG;
}


Wave::~Wave()
{

}


void Wave::reset()
{
    Channel::reset();

    volume_shift = 0;

    // Wave
    wave_position = 0;
    wave_clock = 0;
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
    mmu->set_nocheck(NR52, mmu->get(NR52) | enabled_flag);

    ve_timer = ve_period;
    ve_enabled = true;

    reload_length(256);
    set_NR30(mmu->get(NR30));       // Reload DAC (disable channel if DAC disabled)
    set_NR32(mmu->get(NR32));       // Reload volume

    wave_position = 0;
    wave_clock = dmg->get_current_clock();
}


void Wave::frequency_sweep()
{
    // Does nothing
}


/**
 * @brief      Set channel 3 - wave on/off
 * @param[in]  value  The value
 */
void Wave::set_NR30(uint8_t value)
{
    dac_enabled = value & 0x80;
    if (!dac_enabled) {
        disable_dac();
    }
}


/**
 * @brief      Set duration of the wave
 * @param[in]  value Duration of the wave
 */
void Wave::set_NR31(uint8_t value)
{
    length = 256 - value;
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
    set_NRX3(value);
}


/**
 * @brief      Set wave frequency hi and control wave flags
 * @param[in]  value Value of that
 */
void Wave::set_NR34(uint8_t value)
{
    set_NRX4(value);
}


/**
 * @brief      Wave frequency needs conversion to be in Hz
 * @return     The wave frequency.in Hz
 */
size_t Wave::get_frequency()
{
    return (2048 - frequency_raw) * 2;
}


/**
 * @brief      Action to do when powered off, reset wave
 */
void Wave::power_off()
{
    Channel::power_off();

    wave_position = 0;
}


void Wave::adjust_clocks(size_t adjustment)
{
    wave_clock -= adjustment;
}


void Wave::serialize(std::ofstream &file)
{
    Channel::serialize(file);

    file.write(reinterpret_cast<char*>(&volume_shift), sizeof(size_t));
    file.write(reinterpret_cast<char*>(&wave_position), sizeof(size_t));
    file.write(reinterpret_cast<char*>(&wave_clock), sizeof(size_t));
}


void Wave::deserialize(std::ifstream &file)
{
    Channel::deserialize(file);

    file.read(reinterpret_cast<char*>(&volume_shift), sizeof(size_t));
    file.read(reinterpret_cast<char*>(&wave_position), sizeof(size_t));
    file.read(reinterpret_cast<char*>(&wave_clock), sizeof(size_t));
}
