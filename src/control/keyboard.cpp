#include "keyboard.h"

Keyboard::Keyboard() {
    forward = ALLEGRO_KEY_D;
    backward = ALLEGRO_KEY_A;
    jump = ALLEGRO_KEY_O;
}

void Keyboard::key_down(int key) {
    pressed_keys.insert(key);
}

void Keyboard::key_up(int key) {
    pressed_keys.erase(key);
}

bool Keyboard::is_key_down(int key) const {
    return pressed_keys.find(key) != pressed_keys.end();
}

bool Keyboard::is_forward_pressed() const {
    return is_key_down(forward);
}

bool Keyboard::is_backward_pressed() const {
    return is_key_down(backward);
}

bool Keyboard::is_jump_pressed() const {
    return is_key_down(jump);
}
