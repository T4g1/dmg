#include "sound.h"

#include "log.h"


Sound::Sound() : mmu(nullptr)
{
    clock = 0;
    last_sequencer_increment = 0;
    last_downsample = 0;

    sample_count = 0;

    current_step = 0;

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

    // Wave - Channel 1
    pa_sweep_time = 0;
    pa_sweep_decrease = false;
    pa_sweep_shift = 0;
    pa_wave_duty = 0;
    pa_length = 0;
    pa_volume = 0;
    pa_envelope_increase = false;
    pa_envelope_count = 0;
    pa_frequency = 0;
    pa_restart = false;
    pa_length_limited = false;
    pa_length_counter = 0;

    pa_output = 0;
    pa_frequency_timer = 0;
    pa_position = 0;

    // Wave - Channel 3
    wave_playback = false;
    wave_length = 0;
    wave_output_level = 0;
    wave_frequency = 0;
    wave_restart = false;
    wave_length_limited = false;
    wave_length_counter = 0;

    wave_output = 0;
    wave_frequency_timer = 0;
    wave_position = 0;
}


Sound::~Sound()
{
    SDL_CloseAudioDevice(audio_device);
}


bool Sound::init()
{
    if (mmu == nullptr) {
        error("No MMU linked to Sound\n");
        return false;
    }

    SDL_AudioSpec audio_spec;
    SDL_zero(audio_spec);
    audio_spec.freq = SOUND_FREQUENCY;
    audio_spec.format = AUDIO_U8;
    audio_spec.channels = 1;
    audio_spec.samples = SOUND_WAVE_REG_COUNT / 2;  // 32 registers of 4 bit so 16 bytes

     audio_device = SDL_OpenAudioDevice(NULL, 0, &audio_spec, NULL, 0);
     if (audio_device <= 0) {
        error("Erreur d'ouverture audio: %s\n", SDL_GetError());
        return false;
    }

    return true;
}


void Sound::reset()
{
    // TODO
}


/**
 * @brief      Called each SOUND_CLOCK_STEP clock ticks
 */
void Sound::step()
{
    clock += SOUND_CLOCK_STEP;

    update();

    if (clock >= last_downsample) {
        downsample();
    }

    if (clock >= last_sequencer_increment) {
        frame_sequencer();
    }
}


void Sound::update()
{
    pulse_a_update();
    //pulse_b_update();
    wave_update();
    //noise_update();
}


/**
 * @brief      Adds a sound sample in the buffer (nearest neighboor on the audio sampler)
 */
void Sound::downsample()
{
    last_downsample += SOUND_CLOCK_DOWNSAMPLE;

    sample[sample_count]  = pa_output;
    sample[sample_count] += wave_output;

    sample_count += 1;

    // Buffer full, send to audio
    if (sample_count >= SOUND_DOWNSAMPLE_BUFFER_SIZE) {
        SDL_QueueAudio(audio_device, sample, sample_count);
        SDL_PauseAudioDevice(audio_device, 0);

        while (SDL_GetQueuedAudioSize(audio_device) > SOUND_DOWNSAMPLE_BUFFER_SIZE * 2) {
            SDL_Delay(5);
        }

        sample_count = 0;
    }
}


/**
 * @brief      Update the frame sequencer
 */
void Sound::frame_sequencer()
{
    last_sequencer_increment += SOUND_CLOCK_FRAME_SEQ;

    // Clock length of each channel
    wave_length_counter += 1;
    // TODO: Same for other channels

    if (current_step == 7) {
        pulse_a_enveolope_update();
    } else if (current_step == 2 || current_step == 6) {
        // TODO: Clock sweep
    }

    current_step = (current_step + 1) % SOUND_STEPS;
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
        debug("Vin to SO1\n");
    }

    if (vin_so2) {
        debug("Vin to SO2\n");
    }

    debug("SO1 level: %zu\n", so1_level);
    debug("SO2 level: %zu\n", so2_level);
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

    debug("Sound outputs sets\n");
}


void Sound::set_NR52(uint8_t value)
{
    bool previous_state = activated;

    activated = value & 0b10000000;

    if (activated != previous_state) {
        if (activated) {
            debug("Sound activated\n");
            current_step = 0;
        } else {
            debug("Sound de-activated\n");
            for (uint16_t address=NR10; address<NR51; address++) {
                mmu->set(address, 0x00);
            }
        }
    }
}


bool Sound::is_power_on()
{
    return activated;
}


/*******************************************************************************
 *
 * CHANNEL 1 - PULSE A
 *
 ******************************************************************************/


/**
 * @brief      Update output of pulse A
 */
void Sound::pulse_a_update()
{
    // Duty look-up table
    const bool dutys[SOUND_PULSE_A_DUTY_COUNT][SOUND_PULSE_A_DUTY_SIZE] = {
        {false, true,  true,  true,  true,  true,  true,  true},
        {false, false, true,  true,  true,  true,  true,  true},
        {false, false, false, false, true,  true,  true,  true},
        {false, false, false, false, false, false, true,  true}
    };

    // Length
    if (pa_length_limited && pa_length_counter >= pa_length) {
        pa_length_counter = 0;
    }

    if (pa_restart) {
        pa_restart = false;

        mmu->set_nocheck(NR52, mmu->get(NR52) | SOUND_PULSE_A_ON_FLAG);

        pa_position = 0;
    }

    // Step the duty position
    static size_t steps = 0;
    steps += SOUND_CLOCK_STEP;
    if (pa_frequency_timer >= steps) {
        pa_frequency_timer -= steps;
    } else {
        steps -= pa_frequency_timer;
        pa_frequency_timer = 0;
    }

    if (pa_frequency_timer <= 0) {
        pa_frequency_timer = get_pulse_a_frequency();

        pa_position = (pa_position + 1) % SOUND_PULSE_A_DUTY_SIZE;

        // Generate the output
        pa_output = dutys[pa_wave_duty][pa_position] * pa_volume;
    }
}


/**
 * @brief      Adjust envelope
 */
void Sound::pulse_a_enveolope_update()
{
    if (pa_envelope_count == 0) {
        return;
    }

    if (pa_envelope_increase) {
        pa_volume += 1;

        // Overflow
        if (pa_volume > 0x0F) {
            pa_volume = 0x0F;
        }
    } else {
        pa_volume -= 1;

        // Underflow
        if (pa_volume > 0x0F) {
            pa_volume = 0x00;
        }
    }

    pa_envelope_count -= 1;
}


/**
 * @brief      Reads sweep related registers
 * @param[in]  value  The value
 */
void Sound::set_NR10(uint8_t value)
{
    pa_sweep_time = (value & 0b01110000) >> 4;
    pa_sweep_decrease = value & 0b00001000;
    pa_sweep_shift = value & 0b00000111;

    info("PA Sweep time: %zu\n", pa_sweep_time);
    info("PA Sweep shift: %zu\n", pa_sweep_shift);

    if (pa_sweep_decrease) {
        info("PA Sweep decrease\n");
    } else {
        info("PA Sweep increase\n");
    }
}


/**
 * @brief      Wave duty and length
 * @param[in]  value  The value
 */
void Sound::set_NR11(uint8_t value)
{
    pa_wave_duty = (value & 0b11000000) >> 6;
    pa_length = value & 0b00011111;

    info("PA Sweep time: %zu\n", pa_sweep_time);
    info("PA Length: %zu\n", pa_length);
}


/**
 * @brief      Volume
 * @param[in]  value  The value
 */
void Sound::set_NR12(uint8_t value)
{
    pa_volume = (value & 0b11110000) >> 4;
    pa_envelope_increase = value & 0b00001000;
    pa_envelope_count = value & 0b00000111;

    info("PA volume: %zu\n", pa_volume);
    info("PA envelope count: %zu\n", pa_envelope_count);

    if (pa_envelope_increase) {
        info("PA Envelope increase\n");
    } else {
        info("PA Envelope decrease\n");
    }
}


/**
 * @brief      Frequency lo
 * @param[in]  value  The value
 */
void Sound::set_NR13(uint8_t value)
{
    pa_frequency = (pa_frequency & 0x0700) + value;

    debug("PA frequency raw: %u\n", pa_frequency);
}


/**
 * @brief      Restart, stop on end of length and frequency hi
 * @param[in]  value  The value
 */
void Sound::set_NR14(uint8_t value)
{
    pa_restart = value & 0b10000000;
    pa_length_limited = value & 0b01000000;

    uint16_t frequency_hi = (value & 0b00000111) << 8;
    uint16_t frequency_lo = (pa_frequency & 0x00FF);
    pa_frequency = frequency_hi + frequency_lo;

    if (pa_restart) {
        debug("PA restarts!\n");
    }

    if (pa_length_limited) {
        debug("PA will fade out after given length\n");
    } else {
        debug("PA will not fade out\n");
    }

    debug("PA frequency raw: %u\n", pa_frequency);
}


/**
 * @brief      Pulse A frequency
 * @return     The PA frequency.in Hz
 */
size_t Sound::get_pulse_a_frequency()
{
    return (0x20000 / (0x0800 - pa_frequency)) * SOUND_CLOCK_STEP;
}




/*******************************************************************************
 *
 * CHANNEL 3 - WAVE
 *
 ******************************************************************************/


/**
 * @brief      Handle Wave
 */
void Sound::wave_update()
{
    // Length
    if (wave_length_limited && wave_length_counter >= wave_length) {
        wave_playback = false;
        wave_length_counter = 0;
    }

    if (wave_restart) {
        wave_restart = false;

        mmu->set_nocheck(NR52, mmu->get(NR52) | SOUND_WAVE_ON_FLAG);

        wave_position = 0;
    }

    // Step the wave position
    static size_t steps = 0;
    steps += SOUND_CLOCK_STEP;
    if (wave_frequency_timer >= steps) {
        wave_frequency_timer -= steps;
    } else {
        steps -= wave_frequency_timer;
        wave_frequency_timer = 0;
    }

    if (wave_frequency_timer <= 0) {
        wave_frequency_timer = get_wave_frequency();

        wave_position = (wave_position + 1) % SOUND_WAVE_REG_COUNT;

        // Generate the output
        uint16_t address = SOUND_WAVE_REG_START + (wave_position >> 1);
        uint8_t value = mmu->get(address);
        if (wave_position & 0x01) {
            wave_output = value & 0x0F;
        } else {
            wave_output = value >> SOUND_WAVE_REG_SIZE;
        }

        wave_output = wave_output >> wave_output_level; // Apply sound level

        // Mute Wave
        if (!wave_playback) {
            wave_output = 0;
        }
    }
}


/**
 * @brief      Set channel 3 - wave on/off
 * @param[in]  value  The value
 */
void Sound::set_NR30(uint8_t value)
{
    const uint8_t activate_mask = 0x80;

    wave_playback = value & activate_mask;

    if (wave_playback) {
        debug("Wave activated\n");
    } else {
        debug("Wave de-activated\n");
    }
}


/**
 * @brief      Set duration of the wave
 * @param[in]  value Duration of the wave
 */
void Sound::set_NR31(uint8_t value)
{
    wave_length = value;
    debug("Wave length: %zu\n", wave_length);
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

    debug("Wave output level: %zu%%\n", 100 - (25 * wave_output_level));
}


/**
 * @brief      Lower 8bits of the frequency
 * @param[in]  value  Value of those lower 8 bits
 */
void Sound::set_NR33(uint8_t value)
{
    wave_frequency = (wave_frequency & 0x0700) + value;

    debug("Wave frequency raw: %u\n", wave_frequency);
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
        debug("Wave restarts!\n");
    }

    if (wave_length_limited) {
        debug("Wave will fade out after given length\n");
    } else {
        debug("Wave will not fade out\n");
    }

    debug("Wave frequency raw: %u\n", wave_frequency);
}


/**
 * @brief      Wave frequency needs conversion to be in Hz
 * @return     The wave frequency.in Hz
 */
size_t Sound::get_wave_frequency()
{
    return (0x010000 / (0x0800 - wave_frequency)) * SOUND_CLOCK_STEP;
}
