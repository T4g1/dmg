#include "sound.h"

#include "log.h"


Sound::Sound() : mmu(nullptr)
{
    wave_playback = false;
    wave_length = 0;
    wave_output_level = 0;
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


void Sound::set_NR33(uint8_t /*value*/)
{

}


void Sound::set_NR34(uint8_t /*value*/)
{

}
