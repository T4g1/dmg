#ifndef UTILS_H
#define UTILS_H

#include <inttypes.h>
#include <stdbool.h>
#include <cstddef>
#include <SDL2/SDL.h>
#include <GL/gl3w.h>

#include "imgui.h"

#define UTIL_LEFT       true
#define UTIL_RIGHT      false

bool get_bit(uint8_t byte, size_t offset);
uint8_t set_bit(uint8_t byte, size_t offset, bool value);
uint8_t rotate(uint8_t byte, bool left, bool *carry);
uint8_t shift(uint8_t byte, bool left, bool *carry);
uint8_t swap(uint8_t byte);
void dec16(uint8_t *address);
void inc16(uint8_t *address);
uint16_t char_to_hex(const char *value);

uint8_t get_pixel_value(uint8_t data1, uint8_t data2, size_t index);
void set_pixel(SDL_Surface *surface, size_t x, size_t y, Uint32 color);
GLuint create_texture(uint8_t data[], size_t width, size_t height);

#endif /* UTILS_H */
