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

    reset();

    return true;
}


void Input::reset()
{
    a_pressed = false;
    b_pressed = false;
    start_pressed = false;
    select_pressed = false;

    up_pressed = false;
    down_pressed = false;
    right_pressed = false;
    left_pressed = false;
}


/**
 * @brief      Update registers
 */
void Input::update()
{
    uint8_t joypad = mmu->get(JOYPAD);

    if (joypad & SELECT_DIRECTION_KEY_MASKS) {
        joypad = set_bit(joypad, KEY_DOWN_START, !down_pressed);     // 0 = pressed
        joypad = set_bit(joypad, KEY_UP_SELECT, !up_pressed);
        joypad = set_bit(joypad, KEY_LEFT_B, !left_pressed);
        joypad = set_bit(joypad, KEY_RIGHT_A, !right_pressed);
    }
    if (joypad & SELECT_BUTTON_KEY_MASKS) {
        joypad = set_bit(joypad, KEY_DOWN_START, !start_pressed);    // 0 = pressed
        joypad = set_bit(joypad, KEY_UP_SELECT, !select_pressed);
        joypad = set_bit(joypad, KEY_LEFT_B, !b_pressed);
        joypad = set_bit(joypad, KEY_RIGHT_A, !a_pressed);
    }

    mmu->set(JOYPAD, joypad);

    // Set joypad interrupt
    if (interrupt_request) {
        mmu->set(IF_ADDRESS, mmu->get(IF_ADDRESS) | INT_JOYPAD_MASK);

        interrupt_request = false;
    }
}


void Input::handle(SDL_Event *event)
{
    SDL_Keycode code;

    switch(event->type) {
    case SDL_KEYDOWN:
        code = event->key.keysym.sym;
        switch(code){
        case SDLK_LEFT:     interrupt_request = true; left_pressed = true;    break;
        case SDLK_RIGHT:    interrupt_request = true; right_pressed = true;   break;
        case SDLK_UP:       interrupt_request = true; up_pressed = true;      break;
        case SDLK_DOWN:     interrupt_request = true; down_pressed = true;    break;
        case SDLK_a:        interrupt_request = true; a_pressed = true;       break;
        case SDLK_b:        interrupt_request = true; b_pressed = true;       break;
        case SDLK_KP_SPACE: interrupt_request = true; start_pressed = true;   break;
        case SDLK_KP_ENTER: interrupt_request = true; select_pressed = true;  break;
        }
        break;

    case SDL_KEYUP:
        code = event->key.keysym.sym;
        switch(event->key.keysym.sym){
        case SDLK_LEFT:     left_pressed = false;   break;
        case SDLK_RIGHT:    right_pressed = false;  break;
        case SDLK_UP:       up_pressed = false;     break;
        case SDLK_DOWN:     down_pressed = false;   break;
        case SDLK_a:        a_pressed = false;      break;
        case SDLK_b:        b_pressed = false;      break;
        case SDLK_KP_SPACE: start_pressed = false;  break;
        case SDLK_KP_ENTER: select_pressed = false; break;
        }
        break;
    }
}


void Input::set_mmu(MMU *mmu)
{
    this->mmu = mmu;
}
