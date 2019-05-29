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
    wave_update();
    // TODO: Other channel
}


/**
 * @brief      Adds a sound sample in the buffer (nearest neighboor on the audio sampler)
 */
void Sound::downsample()
{
    last_downsample += SOUND_CLOCK_DOWNSAMPLE;

    sample[sample_count++] = wave_output;

    // Buffer full, send to audio
    if (sample_count >= SOUND_DOWNSAMPLE_BUFFER_SIZE) {
        SDL_QueueAudio(audio_device, sample, sample_count);
        SDL_PauseAudioDevice(audio_device, 0);

        while (SDL_GetQueuedAudioSize(audio_device) > SOUND_DOWNSAMPLE_BUFFER_SIZE * 2) {
            SDL_Delay(10);
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
        // TODO: Clock the enveloppe
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

    // Mute Wave
    if (!wave_playback) {

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

        //info("Wave out: 0x%02X\n", wave_output);
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
    const uint8_t length_mask = 0x7F;

    wave_length = value & length_mask;
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
    return (2048- wave_frequency) * 2;
}
