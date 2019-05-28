#ifndef SOUND_H
#define SOUND_H

#include "defines.h"
#include "mmu.h"


/**
 * @brief      Hanldes sounds
 */
class Sound {
    MMU *mmu;

    // Channel 3 - Wave
    bool wave_playback;
    size_t wave_length;
    size_t wave_output_level;       // As a shift count
public:
    Sound();
    ~Sound();

    bool init();
    void reset();
    void update();

    void set_mmu(MMU *mmu);

    void serialize(std::ofstream &file);
    void deserialize(std::ifstream &file);

    // Channel 3 - Wave
    void set_NR30(uint8_t value);
    void set_NR31(uint8_t value);
    void set_NR32(uint8_t value);
    void set_NR33(uint8_t value);
    void set_NR34(uint8_t value);
};

#endif /* SOUND_H */
