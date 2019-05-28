#include "sound.h"

#include "log.h"


Sound::Sound() : mmu(nullptr)
{
    // Sound control
    activated = false;
    vin_so1 = false;
    vin_so2 = false;
    pulse_a_so1 = false;
    pulse_a_so2 = false;
    pulse_b_so1 = false;
    pulse_b_so2 = false;
    wave_so1 = false;
    wave_so2 = false;
    noise_so1 = false;
    noise_so2 = false;
    so1_level = 0;
    so2_level = 0;

    // Wave - Channel 3
    wave_playback = false;
    wave_length = 0;
    wave_output_level = 0;
    wave_frequency = 0;
    wave_restart = false;
    wave_length_limited = false;
}


Sound::~Sound()
{

}


bool Sound::init()
{
    if (mmu == nullptr) {
        error("No MMU linked to Sound\n");
        return false;
    }

    return true;
}


void Sound::reset()
{
    // TODO
}


void Sound::update()
{
    // TODO
}


void Sound::set_mmu(MMU *mmu)
{
    this->mmu = mmu;
}


void Sound::serialize(std::ofstream &/*file*/)
{
    // TODO
}


void Sound::deserialize(std::ifstream &/*file*/)
{
    // TODO
}


/*******************************************************************************
 *
 * CONTROL
 *
 ******************************************************************************/


/**
 * @brief      Set level and Vin output
 * @param[in]  value  The value of those things
 */
void Sound::set_NR50(uint8_t value)
{
    vin_so1 = value & 0b10000000;
    vin_so2 = value & 0b00001000;

    so1_level = (value & 0b01110000) >> 4;
    so2_level = value & 0b00000111;

    if (vin_so1) {
        info("Vin to SO1\n");
    }

    if (vin_so2) {
        info("Vin to SO2\n");
    }

    info("SO1 level: %zu\n", so1_level);
    info("SO2 level: %zu\n", so2_level);
}


/**
 * @brief      All channels outputs
 * @param[in]  value  The value
 */
void Sound::set_NR51(uint8_t value)
{
    noise_so2 = value & 0b10000000;
    wave_so2 = value & 0b01000000;
    pulse_b_so2 = value & 0b001000000;
    pulse_a_so2 = value & 0b000100000;
    noise_so1 = value & 0b00001000;
    wave_so1 = value & 0b00000100;
    pulse_b_so1 = value & 0b00000010;
    pulse_a_so1 = value & 0b00000001;

    info("Sound outputs sets\n");
}


void Sound::set_NR52(uint8_t value)
{
    activated = value & 0b10000000;

    if (activated) {
        info("Sound activated\n");
    } else {
        info("Sound de-activated\n");
    }
}


/*******************************************************************************
 *
 * CHANNEL 3 - WAVE
 *
 ******************************************************************************/


/**
 * @brief      Set channel 3 - wave on/off
 * @param[in]  value  The value
 */
void Sound::set_NR30(uint8_t value)
{
    const uint8_t activate_mask = 0x80;

    wave_playback = value & activate_mask;

    if (wave_playback) {
        info("Wave activated\n");
    } else {
        info("Wave de-activated\n");
    }
}


/**
 * @brief      Set duration of the wave
 * @param[in]  value Duration of the wave
 */
void Sound::set_NR31(uint8_t value)
{
    const uint8_t length_mask = 0x7F;

    wave_length = value & length_mask;
    info("Wave length: %zu\n", wave_length);
}


/**
 * @brief      Set level of output as a shift count
 * Wave data is on 4 bits so shift 4 times give no sound.
 * @param[in]  value  Output level
 */
void Sound::set_NR32(uint8_t value)
{
    const uint8_t output_level_mask = 0x60;

    wave_output_level = (value & output_level_mask) >> 5;
    if (wave_output_level == 0) {
        wave_output_level = 4;
    } else {
        wave_output_level -= 1;
    }

    info("Wave output level: %zu%%\n", 100 - (25 * wave_output_level));
}


/**
 * @brief      Lower 8bits of the frequency
 * @param[in]  value  Value of those lower 8 bits
 */
void Sound::set_NR33(uint8_t value)
{
    wave_frequency = (wave_frequency & 0x0700) + value;

    info("Wave frequency raw: %u\n", wave_frequency);
}


/**
 * @brief      Set wave frequency hi and control wave flags
 * @param[in]  value Value of that
 */
void Sound::set_NR34(uint8_t value)
{
    wave_restart = value & 0b10000000;
    wave_length_limited = value & 0b01000000;

    uint16_t frequency_hi = (value & 0b00000111) << 8;
    uint16_t frequency_lo = (wave_frequency & 0x00FF);
    wave_frequency = frequency_hi + frequency_lo;

    if (wave_restart) {
        info("Wave restarts!\n");
    }

    if (wave_length_limited) {
        info("Wave will fade out after given length\n");
    } else {
        info("Wave will not fade out\n");
    }

    info("Wave frequency raw: %u\n", wave_frequency);
}


/**
 * @brief      Wave frequency needs conversion to be in Hz
 * @return     The wave frequency.in Hz
 */
size_t Sound::get_wave_frequency()
{
    return 0x10000 / (0x0800 - wave_frequency);
}
