#ifndef SOUND_H
#define SOUND_H

#include "defines.h"
#include "mmu.h"


/**
 * @brief      Hanldes sounds
 */
class Sound {
    MMU *mmu;

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

    // Channel 3 - Wave
    bool wave_playback;
    size_t wave_length;
    size_t wave_output_level;       // As a shift count
    uint16_t wave_frequency;        // This is not in Hz, see get_wave_frequency for Hz
    bool wave_restart;
    bool wave_length_limited;

public:
    Sound();
    ~Sound();

    bool init();
    void reset();
    void update();

    void set_mmu(MMU *mmu);

    void serialize(std::ofstream &file);
    void deserialize(std::ifstream &file);

    void set_NR50(uint8_t value);
    void set_NR51(uint8_t value);
    void set_NR52(uint8_t value);

    // Channel 3 - Wave
    void set_NR30(uint8_t value);
    void set_NR31(uint8_t value);
    void set_NR32(uint8_t value);
    void set_NR33(uint8_t value);
    void set_NR34(uint8_t value);

    size_t get_wave_frequency();
};

#endif /* SOUND_H */
