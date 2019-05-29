#ifndef SOUND_H
#define SOUND_H

#include <SDL2/SDL.h>

#include "defines.h"
#include "mmu.h"


/**
 * @brief      Hanldes sounds
 */
class Sound {
    MMU *mmu;

    SDL_AudioSpec audio_spec;
    SDL_AudioDeviceID audio_device;

    size_t last_downsample;             // Time of next sample to take
    size_t last_sequencer_increment;    // Time of last step increment for frame sequencer

    size_t sample_count;
    uint8_t sample[SOUND_DOWNSAMPLE_BUFFER_SIZE];   // Todo: one for each channel?

    size_t current_step;    // Frame sequencer step

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

    // Channel 1 - Pulse A
    size_t pa_sweep_time;
    bool pa_sweep_decrease;         // In case it is false, sweep increase
    size_t pa_sweep_shift;
    size_t pa_wave_duty;            // Which duty wave to use
    size_t pa_length;               // What length start to use
    size_t pa_volume;               // Duty multiply this to get actual output of channel
    bool pa_envelope_increase;
    size_t pa_envelope_count;
    uint16_t pa_frequency;          // This is not in Hz, see get_pulse_a_frequency for Hz
    bool pa_restart;                // Ask to reset pulse A
    bool pa_length_limited;         // Does stops after lenght_counter reach zero?
    size_t pa_length_counter;       // How many length left

    uint8_t pa_output;              // Current channel output
    size_t pa_frequency_timer;      // Pulse A change when this hits zero
    size_t pa_position;             // Which step of the duty is the current one

    // Channel 3 - Wave
    bool wave_playback;             // Control DAC for Wave
    size_t wave_length;             // What length start to use
    size_t wave_output_level;       // As a shift count
    uint16_t wave_frequency;        // This is not in Hz, see get_wave_frequency for Hz
    bool wave_restart;              // Ask to reset wave
    bool wave_length_limited;       // Does stops after lenght_counter reach zero?
    size_t wave_length_counter;     // How many length left

    uint8_t wave_output;            // Current channel output
    size_t wave_frequency_timer;    // Wave position change when this hits zero
    size_t wave_position;           // Index of next wave register to load

public:
    size_t clock;

    Sound();
    ~Sound();

    bool init();
    void reset();
    void step();
    void update();

    void downsample();
    void frame_sequencer();

    void set_mmu(MMU *mmu);

    void serialize(std::ofstream &file);
    void deserialize(std::ifstream &file);

    void set_NR50(uint8_t value);
    void set_NR51(uint8_t value);
    void set_NR52(uint8_t value);

    bool is_power_on();

    // Channel 1 - Pulse A
    void pulse_a_update();
    void pulse_a_enveolope_update();
    void set_NR10(uint8_t value);
    void set_NR11(uint8_t value);
    void set_NR12(uint8_t value);
    void set_NR13(uint8_t value);
    void set_NR14(uint8_t value);

    size_t get_pulse_a_frequency();

    // Channel 3 - Wave
    void wave_update();
    void set_NR30(uint8_t value);
    void set_NR31(uint8_t value);
    void set_NR32(uint8_t value);
    void set_NR33(uint8_t value);
    void set_NR34(uint8_t value);

    size_t get_wave_frequency();

};

#endif /* SOUND_H */
