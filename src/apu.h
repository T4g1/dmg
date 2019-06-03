#ifndef APU_H
#define APU_H

#include <SDL2/SDL.h>

#include "defines.h"
#include "mmu.h"
#include "channels/pulse_a.h"
#include "channels/pulse_b.h"
#include "channels/wave.h"
#include "channels/noise.h"


/**
 * @brief      Hanldes sounds (audio processing unit)
 */
class APU {
    MMU *mmu;

    PulseA pulse_a;
    PulseB pulse_b;
    Wave wave;
    Noise noise;

    SDL_AudioSpec audio_spec;
    SDL_AudioDeviceID audio_device;

    // Downsampler
    size_t downsample_clock;        // Determines when to take a sample
    size_t buffer_count;            // Size of occupied buffer (2 incrmeent = one sample)
    int16_t sample[SOUND_DOWNSAMPLE_BUFFER_SIZE]; // 2 * int16_t per sample

    // Sound control
    bool activated;
    bool vin_so1;
    bool vin_so2;
    bool pulse_a_so1;
    bool pulse_a_so2;
    bool pulse_b_so1;
    bool pulse_b_so2;
    bool wave_so1;
    bool wave_so2;
    bool noise_so1;
    bool noise_so2;

    size_t so1_level;
    size_t so2_level;

public:
    size_t clock;

    bool play_pulse_a;
    bool play_pulse_b;
    bool play_wave;
    bool play_noise;

    APU();
    ~APU();

    bool init();
    void reset();
    void step();
    void update();

    void mixer();
    void downsample();
    void frame_sequencer();

    void set_NR10(uint8_t value) { pulse_a.set_NR10(value); };
    void set_NR11(uint8_t value) { pulse_a.set_NR11(value); };
    void set_NR12(uint8_t value) { pulse_a.set_NR12(value); };
    void set_NR13(uint8_t value) { pulse_a.set_NR13(value); };
    void set_NR14(uint8_t value) { pulse_a.set_NR14(value); };
    void set_NR21(uint8_t value) { pulse_b.set_NR21(value); };
    void set_NR22(uint8_t value) { pulse_b.set_NR22(value); };
    void set_NR23(uint8_t value) { pulse_b.set_NR23(value); };
    void set_NR24(uint8_t value) { pulse_b.set_NR24(value); };
    void set_NR30(uint8_t value) { wave.set_NR30(value); };
    void set_NR31(uint8_t value) { wave.set_NR31(value); };
    void set_NR32(uint8_t value) { wave.set_NR32(value); };
    void set_NR33(uint8_t value) { wave.set_NR33(value); };
    void set_NR34(uint8_t value) { wave.set_NR34(value); };
    void set_NR41(uint8_t value) { noise.set_NR41(value); };
    void set_NR42(uint8_t value) { noise.set_NR42(value); };
    void set_NR43(uint8_t value) { noise.set_NR43(value); };
    void set_NR44(uint8_t value) { noise.set_NR44(value); };

    void set_NR50(uint8_t value);
    void set_NR51(uint8_t value);
    void set_NR52(uint8_t value);

    bool is_power_on();

    void adjust_clocks(size_t adjustment);

    void set_mmu(MMU *mmu);

    void serialize(std::ofstream &file);
    void deserialize(std::ifstream &file);
};

#endif /* APU_H */
