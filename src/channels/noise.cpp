#include "noise.h"

#include "../utils.h"


Noise::Noise()
{
    enabled_flag = SOUND_NOISE_ON_FLAG;
}


Noise::~Noise()
{

}


bool Noise::init()
{
    Channel::init();

    // LFSR (Linear Feedback Shift Register)
    lfsr_value = 0;
    lfsr_clock = 0;
    divisor = 0;
    both_bit = false;
    clock_shift = 0;

    return true;
}


/**
 * @brief      Generate output of LFSR
 */
void Noise::process()
{
    lfsr_clock += get_frequency();

    uint8_t xor_result = get_bit(lfsr_value, 0) ^ get_bit(lfsr_value, 1);

    lfsr_value = lfsr_value >> 1;

    // Set XOR result
    lfsr_value |= xor_result << 14;
    if (both_bit) {
        lfsr_value &= ~0x40;
        lfsr_value |= xor_result << 6;
    }

    if (lfsr_value & 0x01) {
        output = 0;
    } else {
        output = volume;
    }
}


/**
 * @brief      On restart
 */
void Noise::trigger()
{
    enabled = true;
    mmu->set_nocheck(NR52, mmu->get(NR52) | enabled_flag);

    ve_timer = ve_period;
    ve_enabled = true;

    reload_length(64);
    set_NR42(mmu->get(NR42));       // Reload volume

    // TODO: lfsr_clock = now?

    lfsr_value = 0xFFFF;
}


void Noise::frequency_sweep()
{
    // Does nothing
}


/**
 * @brief      Length
 * @param[in]  value  The value
 */
void Noise::set_NR41(uint8_t value)
{
    length = 64 - (value & 0b00111111);
}


/**
 * @brief      Volume
 * @param[in]  value  The value
 */
void Noise::set_NR42(uint8_t value)
{
    volume = (value & 0b11110000) >> 4;
    ve_add = value & 0b00001000;
    ve_period = value & 0b00000111;

    dac_enabled = value & 0b11111000;
    if (!dac_enabled) {
        disable_dac();
    }
}


/**
 * @brief      Lower 8bits of the frequency
 * @param[in]  value  Value of those lower 8 bits
 */
void Noise::set_NR43(uint8_t value)
{
    const size_t divisors[] = {
        8,
        16,
        32,
        48,
        64,
        80,
        96,
        112
    };

    divisor = divisors[value & 0b00000111];
    clock_shift = (value & 0xF0) >> 4;
    both_bit = value & 0b00001000;
}


/**
 * @brief      Restart, stop on end of length
 * @param[in]  value  The value
 */
void Noise::set_NR44(uint8_t value)
{
    set_NRX4(value);
}


size_t Noise::get_frequency()
{
    return divisor << clock_shift;
}


void Noise::adjust_clocks(size_t adjustment)
{
    lfsr_clock -= adjustment;
}


void Noise::serialize(std::ofstream &file)
{
    Channel::serialize(file);

    file.write(reinterpret_cast<char*>(&lfsr_value), sizeof(uint16_t));
    file.write(reinterpret_cast<char*>(&divisor), sizeof(size_t));
    file.write(reinterpret_cast<char*>(&both_bit), sizeof(bool));
    file.write(reinterpret_cast<char*>(&clock_shift), sizeof(size_t));

    file.write(reinterpret_cast<char*>(&lfsr_clock), sizeof(size_t));
}


void Noise::deserialize(std::ifstream &file)
{
    Channel::deserialize(file);

    file.read(reinterpret_cast<char*>(&lfsr_value), sizeof(uint16_t));
    file.read(reinterpret_cast<char*>(&divisor), sizeof(size_t));
    file.read(reinterpret_cast<char*>(&both_bit), sizeof(bool));
    file.read(reinterpret_cast<char*>(&clock_shift), sizeof(size_t));

    file.read(reinterpret_cast<char*>(&lfsr_clock), sizeof(size_t));
}
