#ifndef CHANNEL_H
#define CHANNEL_H

#include "mmu.h"
#include "log.h"


class Channel {
protected:
    MMU *mmu;

    bool enabled;                   // Channel should produce output
    bool restart;                   // Channel should be restarted
    uint8_t volume;                 // Volume of the channel
    uint8_t output;                 // Output of the channel

    // Frame sequencer
    size_t sequencer_step;          // Current step of the frame seqeucner

    // Length counter
    bool length_limitation;         // Do we stop the channel when length reach zero?
    size_t length;                  // Base length to count from

    // Volume Envelope
    bool ve_enabled;                // Envelope can be disabled until restart
    size_t ve_period;               // How many volume adjustment we will do
    size_t ve_timer;                // Used to trigger at the right time
    bool ve_add;                    // Volume is incremented or decremented

    // DAC
    bool dac_enabled;               // Controlled by upper 5 bit of NRX2
    int16_t dac_output;             // Output of DAC

public:
    // Frame sequencer
    size_t sequencer_clock;         // Allow to determine when the frame sequencer should step

    Channel();

    bool init();
    void update();

    virtual void process() = 0;     // Channel related output generation
    virtual void trigger() = 0;     // Channel is restarted

    void frame_sequencer();
    void length_counter();
    void volume_envelope();

    void frequency_sweep();
    //void duty(); // TODO

    void dac();                     // Handles digital to analog converter

    uint8_t get_volume();
    int16_t get_output();

    void set_mmu(MMU *mmu);
};

#endif /* CHANNEL_H */
