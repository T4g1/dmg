#ifndef INPUT_H
#define INPUT_H

#include <SDL2/SDL.h>

#include "mmu.h"

#define SELECT_BUTTON_KEY_MASKS       0b00100000
#define SELECT_DIRECTION_KEY_MASKS    0b00010000
#define KEY_DOWN_START      3
#define KEY_UP_SELECT       2
#define KEY_LEFT_B          1
#define KEY_RIGHT_A         0


/**
 * @brief      Hanldes inputs
 */
class Input {
    MMU *mmu;

    // Indicates that we need to update registers, key status was changed
    bool dirty;
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

    bool get_selected(uint8_t joypad, uint8_t line_mask);
    void set_key(uint8_t *joypad, size_t key, bool pressed);

    void set_mmu(MMU *mmu);
};

#endif /* INPUT_H */
