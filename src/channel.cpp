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

    enabled = false;
    restart = false;
    volume = 0;
    output = 0;

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

    return true;
}


void reset()
{
    // TODO
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
    ve_timer -= 1;
    if (ve_timer != 0) {
        return;
    }

    ve_timer = SOUND_VOLUME_ENVELOPE_FREQ;

    if (!ve_enabled || ve_period == 0) {
        return;
    }

    // Addition
    if (ve_add) {
        if (volume < 0x0F) {
            volume += 1;
            ve_period -= 1;
        } else {
            // Stop operations!
            ve_enabled = false;
        }
    }
    // Substraction
    else {
        if (volume > 0x00) {
            volume -= 1;
            ve_period -= 1;
        } else {
            // Stop operations!
            ve_enabled = false;
        }
    }
}


void Channel::frequency_sweep()
{
    // Only defined for Pulse A (overrided there)
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
