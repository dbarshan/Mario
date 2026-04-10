#pragma once

#include <allegro5/allegro.h>
#include <unordered_set>

class Keyboard {
private:
    std::unordered_set<int> pressed_keys;

public:
    int forward;
    int backward;
    int jump;

    Keyboard();

    void key_down(int key);
    void key_up(int key);
    
    [[nodiscard]] bool is_key_down(int key) const;
    [[nodiscard]] bool is_forward_pressed() const;
    [[nodiscard]] bool is_backward_pressed() const;
    [[nodiscard]] bool is_jump_pressed() const;
};