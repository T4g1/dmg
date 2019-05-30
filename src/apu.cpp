#include "apu.h"

#include "log.h"


APU::APU() : mmu(nullptr)
{
    clock = 0;
    downsample_clock = 0;
    sample_count = 0;

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
}


APU::~APU()
{
    SDL_CloseAudioDevice(audio_device);
}


bool APU::init()
{
    if (mmu == nullptr) {
        error("No MMU linked to APU\n");
        return false;
    }

    SDL_AudioSpec audio_spec;
    SDL_zero(audio_spec);
    audio_spec.freq = SOUND_FREQUENCY;
    audio_spec.format = AUDIO_S16SYS;
    audio_spec.channels = 2;
    audio_spec.samples = SOUND_DOWNSAMPLE_BUFFER_SIZE;

     audio_device = SDL_OpenAudioDevice(NULL, 0, &audio_spec, NULL, 0);
     if (audio_device <= 0) {
        error("Erreur d'ouverture audio: %s\n", SDL_GetError());
        return false;
    }

    SDL_PauseAudioDevice(audio_device, 0);

    pulse_a.set_mmu(mmu);
    pulse_b.set_mmu(mmu);
    wave.set_mmu(mmu);
    //noise.set_mmu(mmu);

    pulse_a.init();
    pulse_b.init();
    wave.init();
    //noise.init();

    return true;
}


void APU::reset()
{
    // TODO
}


/**
 * @brief      Called each SOUND_CLOCK_STEP clock ticks
 */
void APU::step()
{
    clock += SOUND_CLOCK_STEP;

    update();

    if (clock >= downsample_clock) {
        downsample();
    }

    if (clock >= pulse_a.sequencer_clock) {
        pulse_a.frame_sequencer();
    }

    if (clock >= pulse_b.sequencer_clock) {
        pulse_b.frame_sequencer();
    }

    if (clock >= wave.sequencer_clock) {
        wave.frame_sequencer();
    }

    /*if (clock >= noise.sequencer_clock) {
        noise.frame_sequencer();
    }*/
}


void APU::update()
{
    pulse_a.update();
    pulse_b.update();
    wave.update();
    //noise.update();
}


void APU::mixer()
{
    int16_t buffer, result;

    // Left
    int16_t volume = so1_level * (128 / 7.0);
    result = 0;
    buffer = 0;

    if (pulse_a_so1) {
        buffer = pulse_a.get_output();
        SDL_MixAudioFormat((Uint8*)&result, (Uint8*)&buffer, AUDIO_S16SYS, sizeof(int16_t), volume);
    }

    if (pulse_b_so1) {
        buffer = pulse_b.get_output();
        SDL_MixAudioFormat((Uint8*)&result, (Uint8*)&buffer, AUDIO_S16SYS, sizeof(int16_t), volume);
    }

    if (wave_so1) {
        buffer = wave.get_output();
        SDL_MixAudioFormat((Uint8*)&result, (Uint8*)&buffer, AUDIO_S16SYS, sizeof(int16_t), volume);
    }

    /*if (noise_so1) {
        buffer = (noise.get_output() / 100);
        SDL_MixAudioFormat((Uint8*)&result, (Uint8*)&buffer, AUDIO_S16SYS, sizeof(int16_t), volume);
    }*/

    sample[sample_count++] = result * 10;

    // Right
    volume = so2_level * (128 / 7.0);
    result = 0;
    buffer = 0;

    if (pulse_a_so2) {
        buffer = pulse_a.get_output();
        SDL_MixAudioFormat((Uint8*)&result, (Uint8*)&buffer, AUDIO_S16SYS, sizeof(int16_t), volume);
    }

    if (pulse_b_so2) {
        buffer = pulse_b.get_output();
        SDL_MixAudioFormat((Uint8*)&result, (Uint8*)&buffer, AUDIO_S16SYS, sizeof(int16_t), volume);
    }

    if (wave_so2) {
        buffer = wave.get_output();
        SDL_MixAudioFormat((Uint8*)&result, (Uint8*)&buffer, AUDIO_S16SYS, sizeof(int16_t), volume);
    }

    /*if (noise_so2) {
        buffer = (noise.get_output() / 100);
        SDL_MixAudioFormat((Uint8*)&result, (Uint8*)&buffer, AUDIO_S16SYS, sizeof(int16_t), volume);
    }*/
    sample[sample_count++] = result * 10;
}


/**
 * @brief      Adds a sound sample in the buffer (nearest neighboor on the audio sampler)
 */
void APU::downsample()
{
    downsample_clock += SOUND_DOWNSAMPLE_CLOCK_STEP;

    mixer();

    // Buffer full, send to audio
    if (sample_count >= SOUND_DOWNSAMPLE_BUFFER_SIZE) {
        // Let buffer be drained
        while (SDL_GetQueuedAudioSize(audio_device) > SOUND_DOWNSAMPLE_BUFFER_SIZE * sizeof(int16_t)) {
            SDL_Delay(1);
        }

        SDL_QueueAudio(audio_device, sample, SOUND_DOWNSAMPLE_BUFFER_SIZE * sizeof(int16_t));

        sample_count = 0;
    }
}


void APU::set_mmu(MMU *mmu)
{
    this->mmu = mmu;
}


void APU::serialize(std::ofstream &/*file*/)
{
    // TODO
}


void APU::deserialize(std::ifstream &/*file*/)
{
    // TODO
}


/**
 * @brief      Set level and Vin output
 * @param[in]  value  The value of those things
 */
void APU::set_NR50(uint8_t value)
{
    vin_so1 = value & 0b10000000;
    vin_so2 = value & 0b00001000;

    so1_level = (value & 0b01110000) >> 4;
    so2_level = value & 0b00000111;
}


/**
 * @brief      All channels outputs
 * @param[in]  value  The value
 */
void APU::set_NR51(uint8_t value)
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


void APU::set_NR52(uint8_t value)
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


bool APU::is_power_on()
{
    return activated;
}
