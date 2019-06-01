#ifndef WAVE_H
#define WAVE_H

#include "../channel.h"


class Wave : public Channel {
    size_t volume_shift;        // Shift to apply to the wave value

    // Wave
    size_t wave_position;       // Which wave register should be read

public:
    // Wave
    size_t wave_clock;          // Control when the wave need to be stepped

    Wave();
    ~Wave();

    bool init();
    void process();
    void trigger();

    void frequency_sweep();

    void set_NR30(uint8_t value);
    void set_NR31(uint8_t value);
    void set_NR32(uint8_t value);
    void set_NR33(uint8_t value);
    void set_NR34(uint8_t value);

    size_t get_frequency();
};

#endif /* WAVE_H */
