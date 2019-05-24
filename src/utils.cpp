#include <stdio.h>
#include <sstream>
#include <iostream>

#include "utils.h"


bool get_bit(uint8_t byte, size_t offset)
{
    return  byte & (1 << offset);
}

uint8_t set_bit(uint8_t byte, size_t offset, bool value)
{
    if (value) {
        return byte | (1 << offset);
    } else {
        return byte & ~(1 << offset);
    }
}

uint8_t rotate(uint8_t byte, bool left, bool *carry)
{
    if (left) {
        *carry = get_bit(byte, 7);
        return byte << 1;
    } else {
        *carry = get_bit(byte, 0);
        return byte >> 1;
    }
}

uint8_t shift(uint8_t byte, bool left, bool *carry)
{
    if (left) {
        *carry = get_bit(byte, 7);
        return byte << 1;
    } else {
        *carry = get_bit(byte, 0);
        return byte >> 1;
    }
}

uint8_t swap(uint8_t byte)
{
    uint8_t tmp = byte;

    byte = byte >> 4;
    tmp = tmp << 4;

    return byte | tmp;
}

void dec16(uint8_t *address)
{
    uint8_t *high = address;
    uint8_t *low = address + 1;
    *low -= 1;
    if (*low == 0xFF) {
        *high -= 1;
    }
}

void inc16(uint8_t *address)
{
    uint8_t *high = address;
    uint8_t *low = address + 1;
    *low += 1;
    if (*low == 0x00) {
        *high += 1;
    }
}

/**
 * @brief      Converts an hexadecimal value from string to uint16_t
 * @param[in]  value  The hexadecimal string
 * @return     The value in uint16_t
 */
uint16_t char_to_hex(const char *value)
{
    uint16_t result;
    std::stringstream ss;
    ss << std::hex << value;
    ss >> result;

    return result;
}


/**
 * @brief      Get the pixel at given index from the data given
 * @param[in]  data1  First data row of that tile line
 * @param[in]  data2  Second data row of that tile line
 * @param[in]  index  The index of the pixel [0, 7]
 * @return     The pixel value.
 */
uint8_t get_pixel_value(uint8_t data1, uint8_t data2, size_t index)
{
    // 8 first bit (data1) = low bit of data for the pixels
    // 8 last bit (data2) = high bit of data for the pixels
    // b7 b6 b5 b4 b3 b2 b1 b0 so 7 - index to get correct bit
    size_t low = get_bit(data1, 7 - index);
    size_t high = get_bit(data2, 7 - index);

    return (high << 1) + low;
}


void set_pixel(SDL_Surface *surface, size_t x, size_t y, Uint32 color)
{
    Uint32 *pixels = (Uint32 *) surface->pixels;

    pixels[(y * surface->w) + x] = color;
}


/**
 * @brief      Given an array of data, give a texture ID
 * @param      data data to use for pixels array of [3] for RGB
 * @return     ImGui texture ID
 */
GLuint create_texture(uint8_t data[], size_t width, size_t height)
{
    GLuint textureID;
    glGenTextures(1, &textureID);

    glBindTexture(GL_TEXTURE_2D, textureID);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    return textureID;
}
