#ifndef PULSE_B_H
#define PULSE_B_H

#include "pulse_a.h"


class PulseB : public PulseA {
public:
    PulseB();
    ~PulseB();

    void trigger();

    void frequency_sweep();

    void set_NR21(uint8_t value);
    void set_NR22(uint8_t value);
    void set_NR23(uint8_t value);
    void set_NR24(uint8_t value);
};

#endif /* PULSE_B_H */
