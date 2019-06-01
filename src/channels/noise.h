#ifndef NOISE_H
#define NOISE_H

#include "../channel.h"


class Noise : public Channel {
    // LFSR (Linear Feedback Shift Register)
    uint16_t lfsr_value;        // Internal register
    size_t divisor;
    bool both_bit;              // Put XOR result in both bit? or only in 15th?
    size_t clock_shift;         // Determine frequency of noise

public:
    // LFSR (Linear Feedback Shift Register)
    size_t lfsr_clock;

    Noise();
    ~Noise();

    bool init();
    void process();
    void trigger();

    void frequency_sweep();

    void set_NR40(uint8_t value);
    void set_NR41(uint8_t value);
    void set_NR42(uint8_t value);
    void set_NR43(uint8_t value);
    void set_NR44(uint8_t value);

    size_t get_frequency();
};

#endif /* NOISE_H */
