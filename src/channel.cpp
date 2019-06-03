#include "channel.h"

#include "defines.h"
#include "log.h"


Channel::Channel() : mmu(nullptr)
{
    enabled_flag = 0;   // No flag
}


bool Channel::init()
{
    if (mmu == nullptr) {
        error("MMU not linked to Channel!\n");
        return false;
    }

    return true;
}


void Channel::reset()
{
    enabled = false;
    restart = false;
    volume = 0;
    output = 0;

    frequency_raw = 0;

    // Frame sequencer
    sequencer_clock = 0;
    sequencer_step = 0;

    // Length counter
    length_limitation = false;
    length = 0;

    // Volume Envelope
    ve_enabled = true;
    ve_period = 0;
    ve_timer = 0;
    ve_add = false;

    // DAC
    dac_enabled = false;
}


/**
 * @brief      Generates output
 */
void Channel::update()
{
    process();

    dac();
}


/**
 * @brief      Timer for length counter, duty/wave/LSFR and envelope
 */
void Channel::frame_sequencer()
{
    sequencer_clock += SOUND_FRAME_SEQ_CLOCK_STEP;

    // Clock the length counter
    if (!(sequencer_step & 0x01)) {
        length_counter();
    }

    if (sequencer_step == 7) {
        volume_envelope();
    }

    if (sequencer_step == 2 || sequencer_step == 6) {
        frequency_sweep();
    }

    sequencer_step = (sequencer_step + 1) % SOUND_FRAME_SEQ_STEP_COUNT;
}


/**
 * @brief      Decrement length under given conditions and disable channel
 */
void Channel::length_counter()
{
    if (!length_limitation) {
        return;
    }

    if (length > 0) {
        length -= 1;

        if (length <= 0) {
            disable();
        }
    }
}


/**
 * @brief      Handle volume sweep
 */
void Channel::volume_envelope()
{
    if (ve_timer > 0) {
        ve_timer -= 1;
    }

    if (ve_timer > 0) {
        return;
    }

    ve_timer = ve_period;
    if (ve_timer == 0) {
        ve_timer = 8;
    }

    if (!ve_enabled || ve_period == 0) {
        return;
    }

    // Addition
    if (ve_add) {
        if (volume < 0x0F) {
            volume += 1;
        }
    }
    // Substraction
    else {
        if (volume > 0x00) {
            volume -= 1;
        }
    }

    if (volume == 0x00 || volume == 0x0F) {
        ve_enabled = false;
    }
}


/**
 * @brief      Digital to analog converter
 */
void Channel::dac()
{
    if (dac_enabled) {
        dac_output = -1 + (2 / 15.0) * output;
    } else {
        enabled = false;
        dac_output = 0;
    }
}


uint8_t Channel::get_volume()
{
    return volume;
}


int16_t Channel::get_output()
{
    if (enabled) {
        return dac_output;
    }

    return 0;
}


void Channel::set_mmu(MMU *mmu)
{
    this->mmu = mmu;
}


/**
 * @brief      Frequency lo
 * @param[in]  value  The value
 */
void Channel::set_NRX3(uint8_t value)
{
    frequency_raw = (frequency_raw & 0x0700) + value;
}


/**
 * @brief      Restart, stop on end of length and frequency hi
 * @param[in]  value  The value
 */
void Channel::set_NRX4(uint8_t value)
{
    bool old_length_limitation = length_limitation;

    restart = value & 0b10000000;
    length_limitation = value & 0b01000000;

    uint16_t frequency_hi = (value & 0b00000111) << 8;
    uint16_t frequency_lo = (frequency_raw & 0x00FF);
    frequency_raw = frequency_hi + frequency_lo;

    // Extra lenght clocking when enabling length limitation and next
    // frame sequencer step should clock length
    if (old_length_limitation != length_limitation &&
        length_limitation &&
        sequencer_step & 0x01) {
        length_counter();
    }

    if (restart) {
        trigger();
        restart = false;
    }
}


void Channel::disable()
{
    enabled = false;
    mmu->set_nocheck(NR52, mmu->get(NR52) & ~enabled_flag);
}


void Channel::disable_dac()
{
    dac_enabled = false;
    disable();
}


/**
 * @brief      Set length to its maximal value during a trigger
 * @param[in]  max_value  The maximum value
 */
void Channel::reload_length(size_t max_value)
{
    if (length == 0) {
        length = max_value;
        if (sequencer_step & 0x01) {
            length_counter();
        }
    }
}


/**
 * @brief      Action to do when powered off, set frame sequencer step to 0
 */
void Channel::power_off()
{
    sequencer_step = 0;
}


void Channel::serialize(std::ofstream &file)
{
    file.write(reinterpret_cast<char*>(&enabled), sizeof(bool));
    file.write(reinterpret_cast<char*>(&restart), sizeof(bool));
    file.write(reinterpret_cast<char*>(&volume), sizeof(uint8_t));
    file.write(reinterpret_cast<char*>(&output), sizeof(uint8_t));
    file.write(reinterpret_cast<char*>(&enabled_flag), sizeof(uint8_t));

    file.write(reinterpret_cast<char*>(&frequency_raw), sizeof(size_t));

    // Frame sequencer
    file.write(reinterpret_cast<char*>(&sequencer_step), sizeof(size_t));

    // Length counter
    file.write(reinterpret_cast<char*>(&length_limitation), sizeof(bool));
    file.write(reinterpret_cast<char*>(&length), sizeof(size_t));

    // Volume Envelope
    file.write(reinterpret_cast<char*>(&ve_enabled), sizeof(bool));
    file.write(reinterpret_cast<char*>(&ve_period), sizeof(size_t));
    file.write(reinterpret_cast<char*>(&ve_timer), sizeof(size_t));
    file.write(reinterpret_cast<char*>(&ve_add), sizeof(bool));

    // DAC
    file.write(reinterpret_cast<char*>(&dac_enabled), sizeof(bool));
    file.write(reinterpret_cast<char*>(&dac_output), sizeof(int16_t));

    file.write(reinterpret_cast<char*>(&sequencer_clock), sizeof(size_t));
}


void Channel::deserialize(std::ifstream &file)
{
    file.read(reinterpret_cast<char*>(&enabled), sizeof(bool));
    file.read(reinterpret_cast<char*>(&restart), sizeof(bool));
    file.read(reinterpret_cast<char*>(&volume), sizeof(uint8_t));
    file.read(reinterpret_cast<char*>(&output), sizeof(uint8_t));
    file.read(reinterpret_cast<char*>(&enabled_flag), sizeof(uint8_t));

    file.read(reinterpret_cast<char*>(&frequency_raw), sizeof(size_t));

    // Frame sequencer
    file.read(reinterpret_cast<char*>(&sequencer_step), sizeof(size_t));

    // Length counter
    file.read(reinterpret_cast<char*>(&length_limitation), sizeof(bool));
    file.read(reinterpret_cast<char*>(&length), sizeof(size_t));

    // Volume Envelope
    file.read(reinterpret_cast<char*>(&ve_enabled), sizeof(bool));
    file.read(reinterpret_cast<char*>(&ve_period), sizeof(size_t));
    file.read(reinterpret_cast<char*>(&ve_timer), sizeof(size_t));
    file.read(reinterpret_cast<char*>(&ve_add), sizeof(bool));

    // DAC
    file.read(reinterpret_cast<char*>(&dac_enabled), sizeof(bool));
    file.read(reinterpret_cast<char*>(&dac_output), sizeof(int16_t));

    file.read(reinterpret_cast<char*>(&sequencer_clock), sizeof(size_t));
}
