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

    if (get_selected(joypad, SELECT_BUTTON_KEY_MASKS)) {
        set_key(&joypad, KEY_DOWN_START, start_pressed);
        set_key(&joypad, KEY_UP_SELECT, select_pressed);
        set_key(&joypad, KEY_LEFT_B, b_pressed);
        set_key(&joypad, KEY_RIGHT_A, a_pressed);
    }
    else if (get_selected(joypad, SELECT_DIRECTION_KEY_MASKS)) {
        set_key(&joypad, KEY_DOWN_START, down_pressed);
        set_key(&joypad, KEY_UP_SELECT, up_pressed);
        set_key(&joypad, KEY_LEFT_B, left_pressed);
        set_key(&joypad, KEY_RIGHT_A, right_pressed);
    } else {
        set_key(&joypad, KEY_DOWN_START, false);
        set_key(&joypad, KEY_UP_SELECT, false);
        set_key(&joypad, KEY_LEFT_B, false);
        set_key(&joypad, KEY_RIGHT_A, false);
    }

    mmu->set(JOYPAD, joypad);

    // Set joypad interrupt
    if (interrupt_request) {
        mmu->trigger_interrupt(INT_JOYPAD_MASK);

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
        case SDLK_LEFT:     should_interrupt(code); left_pressed = true;    break;
        case SDLK_RIGHT:    should_interrupt(code); right_pressed = true;   break;
        case SDLK_UP:       should_interrupt(code); up_pressed = true;      break;
        case SDLK_DOWN:     should_interrupt(code); down_pressed = true;    break;
        case SDLK_a:        should_interrupt(code); a_pressed = true;       break;
        case SDLK_b:        should_interrupt(code); b_pressed = true;       break;
        case SDLK_SPACE:    should_interrupt(code); start_pressed = true;   break;
        case SDLK_RETURN:   should_interrupt(code); select_pressed = true;  break;
        }
        break;

    case SDL_KEYUP:
        switch(event->key.keysym.sym){
        case SDLK_LEFT:     left_pressed = false;   break;
        case SDLK_RIGHT:    right_pressed = false;  break;
        case SDLK_UP:       up_pressed = false;     break;
        case SDLK_DOWN:     down_pressed = false;   break;
        case SDLK_a:        a_pressed = false;      break;
        case SDLK_b:        b_pressed = false;      break;
        case SDLK_SPACE:    start_pressed = false;  break;
        case SDLK_RETURN:   select_pressed = false; break;
        }
        break;
    }
}


/**
 * @brief      Indicates if the given line is selected or not
 * @param[in]  joypad     The joypad
 * @param[in]  line_mask  The line mask
 * @return     true if it is selected
 */
bool Input::get_selected(uint8_t joypad, uint8_t line_mask)
{
    // 0 = Selected
    return (joypad & line_mask) == 0;
}


/**
 * @brief      Sets the key in the given joypad register
 * @param[in]  joypad   The joypad register value
 * @param[in]  key      The key
 * @param[in]  pressed  The key state (pressed = true)
 */
void Input::set_key(uint8_t *joypad, size_t key, bool pressed)
{
    // Inverted logic for pressed/released (0 = pressed)
    *joypad = set_bit(*joypad, key, !pressed);
}


/**
 * @brief      Determines if an interrupt can occur
 * @param[in]  key   The key being pressed
 */
void Input::should_interrupt(size_t key)
{
    uint8_t joypad = mmu->get(JOYPAD);

    if (get_selected(joypad, SELECT_BUTTON_KEY_MASKS)) {
        if (key == SDLK_a ||
            key == SDLK_b ||
            key == SDLK_SPACE ||
            key == SDLK_RETURN ) {
            interrupt_request = true;
        }
    }

    if (get_selected(joypad, SELECT_DIRECTION_KEY_MASKS)) {
        if (key == SDLK_LEFT ||
            key == SDLK_RIGHT ||
            key == SDLK_UP ||
            key == SDLK_DOWN ) {
            interrupt_request = true;
        }
    }
}


void Input::set_mmu(MMU *mmu)
{
    this->mmu = mmu;
}


void Input::serialize(std::ofstream &/*file*/)
{
    // No need to save state for those
}


void Input::deserialize(std::ifstream &/*file*/)
{
    // No need to load state for those
}
