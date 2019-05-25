#ifndef INPUT_H
#define INPUT_H

#include <SDL2/SDL.h>
#include <iostream>
#include <fstream>

#include "defines.h"
#include "mmu.h"


/**
 * @brief      Hanldes inputs
 */
class Input {
    MMU *mmu;

    bool interrupt_request;
    SDL_Keycode last_key;

    bool a_pressed;
    bool b_pressed;
    bool start_pressed;
    bool select_pressed;

    bool up_pressed;
    bool down_pressed;
    bool right_pressed;
    bool left_pressed;

public:
    Input();

    bool init();
    void reset();
    void update();
    void handle(SDL_Event *event);

    void should_interrupt(size_t key);
    bool get_selected(uint8_t joypad, uint8_t line_mask);
    void set_key(uint8_t *joypad, size_t key, bool pressed);

    void set_mmu(MMU *mmu);

    void serialize(std::ofstream &file);
    void deserialize(std::ifstream &file);
};

#endif /* INPUT_H */
