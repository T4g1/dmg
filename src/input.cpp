#include "input.h"

#include "cpu.h"
#include "utils.h"
#include "log.h"


Input::Input() : mmu(nullptr)
{

}


bool Input::init()
{
    if (mmu == nullptr) {
        error("No MMU linked with Input\n");
        return false;
    }

    a_pressed = false;
    b_pressed = false;
    start_pressed = false;
    select_pressed = false;

    up_pressed = false;
    down_pressed = false;
    right_pressed = false;
    left_pressed = false;

    return true;
}


/**
 * @brief      Update registers
 */
void Input::update()
{
    if (!dirty) {
        return;
    }

    uint8_t joypad = mmu->get(JOYPAD);

    if (last_key == SDLK_LEFT ||
        last_key == SDLK_RIGHT ||
        last_key == SDLK_UP ||
        last_key == SDLK_DOWN) {
        set_bit(joypad, SELECT_BUTTON, 1);
        set_bit(joypad, SELECT_DIRECTION, 0);               // 0 = selected
        set_bit(joypad, KEY_DOWN_START, !down_pressed);     // 0 = pressed
        set_bit(joypad, KEY_UP_SELECT, !up_pressed);
        set_bit(joypad, KEY_LEFT_B, !left_pressed);
        set_bit(joypad, KEY_RIGHT_A, !right_pressed);
    } else {
        set_bit(joypad, SELECT_BUTTON, 0);                  // 0 = selected
        set_bit(joypad, SELECT_DIRECTION, 1);
        set_bit(joypad, KEY_DOWN_START, !start_pressed);    // 0 = pressed
        set_bit(joypad, KEY_UP_SELECT, !select_pressed);
        set_bit(joypad, KEY_LEFT_B, !b_pressed);
        set_bit(joypad, KEY_RIGHT_A, !a_pressed);
    }

    mmu->set(JOYPAD, joypad);

    // Set joypad interrupt
    if (interrupt_request) {
        mmu->set(IF_ADDRESS, mmu->get(IF_ADDRESS) | INT_JOYPAD_MASK);
    }
}


void Input::handle(SDL_Event *event)
{
    SDL_Keycode code;

    switch(event->type) {
    case SDL_KEYDOWN:
        code = event->key.keysym.sym;
        switch(code){
        case SDLK_LEFT:     PRESSED(code, left_pressed);    break;
        case SDLK_RIGHT:    PRESSED(code, right_pressed);   break;
        case SDLK_UP:       PRESSED(code, up_pressed);      break;
        case SDLK_DOWN:     PRESSED(code, down_pressed);    break;
        case SDLK_a:        PRESSED(code, a_pressed);       break;
        case SDLK_b:        PRESSED(code, b_pressed);       break;
        case SDLK_KP_SPACE: PRESSED(code, start_pressed);   break;
        case SDLK_KP_ENTER: PRESSED(code, select_pressed);  break;
        }
        break;

    case SDL_KEYUP:
        code = event->key.keysym.sym;
        switch(event->key.keysym.sym){
        case SDLK_LEFT:     RELEASED(code, left_pressed);    break;
        case SDLK_RIGHT:    RELEASED(code, right_pressed);   break;
        case SDLK_UP:       RELEASED(code, up_pressed);      break;
        case SDLK_DOWN:     RELEASED(code, down_pressed);    break;
        case SDLK_a:        RELEASED(code, a_pressed);       break;
        case SDLK_b:        RELEASED(code, b_pressed);       break;
        case SDLK_KP_SPACE: RELEASED(code, start_pressed);   break;
        case SDLK_KP_ENTER: RELEASED(code, select_pressed);  break;
        }
        break;
    }
}


void Input::set_mmu(MMU *mmu)
{
    this->mmu = mmu;
}
