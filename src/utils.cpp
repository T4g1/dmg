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
GLuint create_texture(float data[], size_t width, size_t height)
{
    GLuint textureID;
    glGenTextures(1, &textureID);

    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_FLOAT, data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    return textureID;
}
