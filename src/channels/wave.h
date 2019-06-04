#ifndef WAVE_H
#define WAVE_H

#include "../channel.h"


class DMG;


class Wave : public Channel {
    DMG *dmg;

    size_t volume_shift;        // Shift to apply to the wave value

    // Wave
    size_t wave_position;       // Which wave register should be read

public:
    // Wave
    size_t wave_clock;          // Control when the wave need to be stepped

    Wave();
    ~Wave();

    void reset();
    void process();
    void trigger();

    void frequency_sweep();

    void set_NR30(uint8_t value);
    void set_NR31(uint8_t value);
    void set_NR32(uint8_t value);
    void set_NR33(uint8_t value);
    void set_NR34(uint8_t value);

    size_t get_frequency();

    uint8_t get_current_wave(size_t position);
    uint8_t read_wave(uint16_t address);
    void write_wave(uint16_t address, uint8_t value);

    void power_off();

    void adjust_clocks(size_t adjustment);

    void serialize(std::ofstream &file);
    void deserialize(std::ifstream &file);

    void set_dmg(DMG *dmg);
};

#endif /* WAVE_H */
