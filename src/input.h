#ifndef INPUT_H
#define INPUT_H

#include <SDL2/SDL.h>

#include "mmu.h"

#define JOYPAD_MASK         0x3F
#define SELECT_BUTTON       5
#define SELECT_DIRECTION    4
#define KEY_DOWN_START      3
#define KEY_UP_SELECT       2
#define KEY_LEFT_B          1
#define KEY_RIGHT_A         0

#define PRESSED(code, variable) \
    variable = true; dirty = true; interrupt_request = true; last_key = code;

#define RELEASED(code, variable) \
    variable = false; dirty = true; last_key = code;


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
    Input(MMU *mmu);

    void update();
    void handle(SDL_Event *event);
};

#endif /* INPUT_H */
