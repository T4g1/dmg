#ifndef PULSE_A_H
#define PULSE_A_H

#include "../channel.h"


class PulseA : public Channel {
    // Wave duty
    size_t duty;                    // Wave duty type to use
    size_t duty_position;           // Position in the duty wave
    size_t duty_frequency;          // Speed for duty steps

    // Frequency Sweep
    size_t sweep_time;              // How many step before applying sweep
    bool sweep_decrease;            // In case it is false, sweep increase
    size_t sweep_shift;             // How many time shift for new frequency
    size_t sweep_time_actual;       // How many sweep update left before applying sweep
    size_t shadow_frequency;
    bool sweep_flag;

public:
    // Wave duty
    size_t duty_clock;              // When do we step the duty

    PulseA();
    ~PulseA();

    bool init();
    void process();
    void trigger();

    void frequency_sweep();

    void set_NR10(uint8_t value);
    void set_NR11(uint8_t value);
    void set_NR12(uint8_t value);
    void set_NR13(uint8_t value);
    void set_NR14(uint8_t value);

    size_t get_frequency();
    size_t compute_frequency(size_t shadow, size_t shift, bool decrease);
};

#endif /* PULSE_A_H */
