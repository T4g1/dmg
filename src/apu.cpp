#include "apu.h"

#include "log.h"


APU::APU() : mmu(nullptr)
{
    clock = 0;
    downsample_clock = 0;
    buffer_count = 0;

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

    play_pulse_a = true;
    play_pulse_b = true;
    play_wave = true;
    play_noise = true;
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
    audio_spec.samples = SOUND_DOWNSAMPLE_SAMPLES;

     audio_device = SDL_OpenAudioDevice(NULL, 0, &audio_spec, NULL, 0);
     if (audio_device <= 0) {
        error("Erreur d'ouverture audio: %s\n", SDL_GetError());
        return false;
    }

    SDL_PauseAudioDevice(audio_device, 0);

    pulse_a.set_mmu(mmu);
    pulse_b.set_mmu(mmu);
    wave.set_mmu(mmu);
    noise.set_mmu(mmu);

    pulse_a.init();
    pulse_b.init();
    wave.init();
    noise.init();

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
    if (clock >= noise.sequencer_clock) {
        noise.frame_sequencer();
    }

    clock = pulse_a.duty_clock;
    if (pulse_b.duty_clock < clock) {
        clock = pulse_b.duty_clock;
    }
    if (wave.wave_clock < clock) {
        clock = wave.wave_clock;
    }
    if (downsample_clock < clock) {
        clock = downsample_clock;
    }
    if (pulse_a.sequencer_clock < clock) {
        clock = pulse_a.sequencer_clock;
    }
    if (pulse_b.sequencer_clock < clock) {
        clock = pulse_b.sequencer_clock;
    }
    if (wave.sequencer_clock < clock) {
        clock = wave.sequencer_clock;
    }
    if (noise.sequencer_clock < clock) {
        clock = noise.sequencer_clock;
    }
}


void APU::update()
{
    if (clock >= pulse_a.duty_clock) {
        pulse_a.update();
    }
    if (clock >= pulse_b.duty_clock) {
        pulse_b.update();
    }
    if (clock >= wave.wave_clock) {
        wave.update();
    }
    if (clock >= noise.lfsr_clock) {
        noise.update();
    }
}


void APU::mixer()
{
    int16_t *result_left = sample + buffer_count++;
    int16_t *result_right = sample + buffer_count++;

    *result_left = 0;
    *result_right = 0;

    int16_t data[] = {
        0, 0, 0, 0
    };

    if (activated) {
        if (play_pulse_a) { data[0] = pulse_a.get_output(); }
        if (play_pulse_b) { data[1] = pulse_b.get_output(); }
        if (play_wave) { data[2] = wave.get_output(); }
        if (play_noise) { data[3] = noise.get_output(); }
    }

    bool to_so1[] = {
        pulse_a_so1,
        pulse_b_so1,
        wave_so1,
        noise_so1
    };

    bool to_so2[] = {
        pulse_a_so2,
        pulse_b_so2,
        wave_so2,
        noise_so2
    };

    // Left
    for (size_t i=0; i<4; i++) {
        if (to_so1[i]) {
            SDL_MixAudioFormat(
                (Uint8*)result_left,
                (Uint8*)&data[i],
                AUDIO_S16SYS,
                sizeof(int16_t),
                so1_level * (128 / 7.0)
            );
        }
    }

    // Right
    for (size_t i=0; i<4; i++) {
        if (to_so2[i]) {
            SDL_MixAudioFormat(
                (Uint8*)result_right,
                (Uint8*)&data[i],
                AUDIO_S16SYS,
                sizeof(int16_t),
                so2_level * (128 / 7.0)
            );
        }
    }

    // Amplify
    *result_left *= 1000;
    *result_right *= 1000;
}


/**
 * @brief      Adds a sound sample in the buffer (nearest neighboor on the audio sampler)
 */
void APU::downsample()
{
    downsample_clock += SOUND_DOWNSAMPLE_CLOCK_STEP;

    mixer();

    // Buffer full, send to audio
    if (buffer_count >= SOUND_DOWNSAMPLE_BUFFER_SIZE) {
        // Let buffer be drained
        while (SDL_GetQueuedAudioSize(audio_device) > SOUND_DOWNSAMPLE_BUFFER_SIZE * sizeof(int16_t)) {
            SDL_Delay(1);
        }

        SDL_QueueAudio(audio_device, sample, SOUND_DOWNSAMPLE_BUFFER_SIZE * sizeof(int16_t));

        buffer_count = 0;
    }
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
    bool new_state = value & 0b10000000;

    if (new_state != previous_state) {
        if (new_state) {
            // TODO: Reset frame sequencers
            // TODO: Reset duty
            // TODO: Reset Wave
        } else {
            for (uint16_t address=NR10; address<NR52; address++) {
                mmu->set(address, 0x00);
            }
        }
    }

    // Set activated now otherwise it would interfere with registers set to 0
    activated = new_state;
}


bool APU::is_power_on()
{
    return activated;
}


void APU::adjust_clocks(size_t adjustment)
{
    clock -= adjustment;
    downsample_clock -= adjustment;

    pulse_a.adjust_clocks(adjustment);
    pulse_b.adjust_clocks(adjustment);
    wave.adjust_clocks(adjustment);
    noise.adjust_clocks(adjustment);
}


void APU::set_mmu(MMU *mmu)
{
    this->mmu = mmu;
}


void APU::serialize(std::ofstream &file)
{
    file.write(reinterpret_cast<char*>(&downsample_clock), sizeof(size_t));
    file.write(reinterpret_cast<char*>(&buffer_count), sizeof(size_t));

    file.write(reinterpret_cast<char*>(sample), sizeof(int16_t) * SOUND_DOWNSAMPLE_BUFFER_SIZE);

    file.write(reinterpret_cast<char*>(&activated), sizeof(bool));
    file.write(reinterpret_cast<char*>(&vin_so1), sizeof(bool));
    file.write(reinterpret_cast<char*>(&vin_so2), sizeof(bool));
    file.write(reinterpret_cast<char*>(&pulse_a_so1), sizeof(bool));
    file.write(reinterpret_cast<char*>(&pulse_a_so2), sizeof(bool));
    file.write(reinterpret_cast<char*>(&pulse_b_so1), sizeof(bool));
    file.write(reinterpret_cast<char*>(&pulse_b_so2), sizeof(bool));
    file.write(reinterpret_cast<char*>(&wave_so1), sizeof(bool));
    file.write(reinterpret_cast<char*>(&wave_so2), sizeof(bool));
    file.write(reinterpret_cast<char*>(&noise_so1), sizeof(bool));
    file.write(reinterpret_cast<char*>(&noise_so2), sizeof(bool));

    file.write(reinterpret_cast<char*>(&so1_level), sizeof(size_t));
    file.write(reinterpret_cast<char*>(&so2_level), sizeof(size_t));

    file.write(reinterpret_cast<char*>(&clock), sizeof(size_t));

    file.write(reinterpret_cast<char*>(&play_pulse_a), sizeof(bool));
    file.write(reinterpret_cast<char*>(&play_pulse_b), sizeof(bool));
    file.write(reinterpret_cast<char*>(&play_wave), sizeof(bool));
    file.write(reinterpret_cast<char*>(&play_noise), sizeof(bool));

    pulse_a.serialize(file);
    pulse_b.serialize(file);
    wave.serialize(file);
    noise.serialize(file);
}


void APU::deserialize(std::ifstream &file)
{
    file.read(reinterpret_cast<char*>(&downsample_clock), sizeof(size_t));
    file.read(reinterpret_cast<char*>(&buffer_count), sizeof(size_t));

    file.read(reinterpret_cast<char*>(sample), sizeof(int16_t) * SOUND_DOWNSAMPLE_BUFFER_SIZE);

    file.read(reinterpret_cast<char*>(&activated), sizeof(bool));
    file.read(reinterpret_cast<char*>(&vin_so1), sizeof(bool));
    file.read(reinterpret_cast<char*>(&vin_so2), sizeof(bool));
    file.read(reinterpret_cast<char*>(&pulse_a_so1), sizeof(bool));
    file.read(reinterpret_cast<char*>(&pulse_a_so2), sizeof(bool));
    file.read(reinterpret_cast<char*>(&pulse_b_so1), sizeof(bool));
    file.read(reinterpret_cast<char*>(&pulse_b_so2), sizeof(bool));
    file.read(reinterpret_cast<char*>(&wave_so1), sizeof(bool));
    file.read(reinterpret_cast<char*>(&wave_so2), sizeof(bool));
    file.read(reinterpret_cast<char*>(&noise_so1), sizeof(bool));
    file.read(reinterpret_cast<char*>(&noise_so2), sizeof(bool));

    file.read(reinterpret_cast<char*>(&so1_level), sizeof(size_t));
    file.read(reinterpret_cast<char*>(&so2_level), sizeof(size_t));

    file.read(reinterpret_cast<char*>(&clock), sizeof(size_t));

    file.read(reinterpret_cast<char*>(&play_pulse_a), sizeof(bool));
    file.read(reinterpret_cast<char*>(&play_pulse_b), sizeof(bool));
    file.read(reinterpret_cast<char*>(&play_wave), sizeof(bool));
    file.read(reinterpret_cast<char*>(&play_noise), sizeof(bool));

    pulse_a.deserialize(file);
    pulse_b.deserialize(file);
    wave.deserialize(file);
    noise.deserialize(file);
}
