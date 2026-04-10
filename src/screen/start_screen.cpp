#include "start_screen.h"
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <iostream>
#include <vector>
#include <memory>
#include <string>

namespace StartScreen {

// Using unique_ptr with custom deleter for Allegro resources
using BitmapPtr = std::unique_ptr<ALLEGRO_BITMAP, void(*)(ALLEGRO_BITMAP*)>;
using DisplayPtr = std::unique_ptr<ALLEGRO_DISPLAY, void(*)(ALLEGRO_DISPLAY*)>;
using TimerPtr = std::unique_ptr<ALLEGRO_TIMER, void(*)(ALLEGRO_TIMER*)>;
using EventQueuePtr = std::unique_ptr<ALLEGRO_EVENT_QUEUE, void(*)(ALLEGRO_EVENT_QUEUE*)>;
using FontPtr = std::unique_ptr<ALLEGRO_FONT, void(*)(ALLEGRO_FONT*)>;

[[nodiscard]] BitmapPtr render_text_bitmap(
    ALLEGRO_FONT* font,
    const char* text,
    ALLEGRO_COLOR color,
    bool bold
) {
    int x, y, w, h;
    al_get_text_dimensions(font, text, &x, &y, &w, &h);
    int width = (w > 1) ? w : 1;
    int height = (h > 1) ? h : 1;
    
    auto* bitmap_ptr = al_create_bitmap(width, height);
    if (!bitmap_ptr) return {nullptr, al_destroy_bitmap};

    BitmapPtr bitmap{bitmap_ptr, al_destroy_bitmap};

    auto* prev_target = al_get_target_bitmap();
    al_set_target_bitmap(bitmap.get());
    al_clear_to_color(al_map_rgba(0, 0, 0, 0));
    
    float draw_x = static_cast<float>(-x);
    float draw_y = static_cast<float>(-y);

    if (bold) {
        auto shadow = al_map_rgb(0, 0, 0);
        al_draw_text(font, shadow, draw_x + 1.0f, draw_y + 1.0f, ALLEGRO_ALIGN_LEFT, text);
        al_draw_text(font, color, draw_x + 1.0f, draw_y, ALLEGRO_ALIGN_LEFT, text);
    }

    al_draw_text(font, color, draw_x, draw_y, ALLEGRO_ALIGN_LEFT, text);
    al_set_target_bitmap(prev_target);

    return bitmap;
}

void draw_start_screen(
    ALLEGRO_BITMAP* background,
    ALLEGRO_BITMAP* mario,
    ALLEGRO_BITMAP* start_text,
    ALLEGRO_BITMAP* options_text,
    ALLEGRO_BITMAP* exit_text,
    ALLEGRO_BITMAP* pointer_text,
    int width,
    int height,
    ALLEGRO_FONT* font,
    MenuChoice selected,
    float mario_x, float mario_y,
    float mario_src_x,
    bool mario_facing_left,
    float sprite_scale
) {
    al_draw_scaled_bitmap(
        background,
        0, 0, static_cast<float>(al_get_bitmap_width(background)), static_cast<float>(al_get_bitmap_height(background)),
        0, 0, static_cast<float>(width), static_cast<float>(height),
        0
    );

    constexpr float sprite_w = 16.0f;
    constexpr float sprite_h = 16.0f;
    const float draw_w = sprite_w * sprite_scale;
    const float draw_h = sprite_h * sprite_scale;

    al_draw_scaled_bitmap(
        mario,
        mario_src_x, 0, sprite_w, sprite_h,
        mario_x, mario_y, draw_w, draw_h,
        mario_facing_left ? ALLEGRO_FLIP_HORIZONTAL : 0
    );

    constexpr float menu_scale = 3.0f;
    const float menu_start_y = static_cast<float>(height) - 400.0f;
    const float menu_gap = (static_cast<float>(al_get_bitmap_height(start_text)) * menu_scale) + 50.0f;

    const float start_w = static_cast<float>(al_get_bitmap_width(start_text)) * menu_scale;
    const float options_w = static_cast<float>(al_get_bitmap_width(options_text)) * menu_scale;
    const float exit_w = static_cast<float>(al_get_bitmap_width(exit_text)) * menu_scale;

    const float menu_x = (static_cast<float>(width) - start_w) / 2.0f;
    al_draw_scaled_bitmap(start_text, 0, 0, static_cast<float>(al_get_bitmap_width(start_text)), static_cast<float>(al_get_bitmap_height(start_text)),
                          menu_x, menu_start_y, start_w, static_cast<float>(al_get_bitmap_height(start_text)) * menu_scale, 0);

    const float options_x = (static_cast<float>(width) - options_w) / 2.0f;
    al_draw_scaled_bitmap(options_text, 0, 0, static_cast<float>(al_get_bitmap_width(options_text)), static_cast<float>(al_get_bitmap_height(options_text)),
                          options_x, menu_start_y + menu_gap, options_w, static_cast<float>(al_get_bitmap_height(options_text)) * menu_scale, 0);

    const float exit_x = (static_cast<float>(width) - exit_w) / 2.0f;
    al_draw_scaled_bitmap(exit_text, 0, 0, static_cast<float>(al_get_bitmap_width(exit_text)), static_cast<float>(al_get_bitmap_height(exit_text)),
                          exit_x, menu_start_y + menu_gap * 2.0f, exit_w, static_cast<float>(al_get_bitmap_height(exit_text)) * menu_scale, 0);

    const float pointer_w = static_cast<float>(al_get_bitmap_width(pointer_text)) * menu_scale;
    const float pointer_h = static_cast<float>(al_get_bitmap_height(pointer_text)) * menu_scale;
    float pointer_x = 0, pointer_y = 0;
    
    switch (selected) {
        case MenuChoice::Start:   pointer_x = menu_x - pointer_w - 12.0f;    pointer_y = menu_start_y; break;
        case MenuChoice::Options: pointer_x = options_x - pointer_w - 12.0f; pointer_y = menu_start_y + menu_gap; break;
        case MenuChoice::Exit:    pointer_x = exit_x - pointer_w - 12.0f;    pointer_y = menu_start_y + menu_gap * 2.0f; break;
    }
    al_draw_scaled_bitmap(pointer_text, 0, 0, static_cast<float>(al_get_bitmap_width(pointer_text)), static_cast<float>(al_get_bitmap_height(pointer_text)),
                          pointer_x, pointer_y, pointer_w, pointer_h, 0);

    constexpr float hud_y = 18.0f;
    constexpr float value_y = hud_y + 28.0f;
    const float hud_left = 24.0f;
    const float hud_right = static_cast<float>(width) - 24.0f;
    const float hud_gap = (hud_right - hud_left) / 3.0f;

    auto white = al_map_rgb(255, 255, 255);
    al_draw_text(font, white, hud_left, hud_y, ALLEGRO_ALIGN_LEFT, "SCORE");
    al_draw_text(font, white, hud_left, value_y, ALLEGRO_ALIGN_LEFT, "000000");
    al_draw_text(font, white, hud_left+hud_gap, hud_y, ALLEGRO_ALIGN_LEFT, "COINS");
    al_draw_text(font, white, hud_left+hud_gap, value_y, ALLEGRO_ALIGN_LEFT, "00");
    al_draw_text(font, white, hud_left+hud_gap*2, hud_y, ALLEGRO_ALIGN_LEFT, "WORLD");
    al_draw_text(font, white, hud_left+hud_gap*2, value_y, ALLEGRO_ALIGN_LEFT, "1-1");
    al_draw_text(font, white, hud_right, hud_y, ALLEGRO_ALIGN_RIGHT, "TIME");
}

MenuChoice run() {
    constexpr int width = 1024;
    constexpr int height = 768;
    
    DisplayPtr display{al_create_display(width, height), al_destroy_display};
    TimerPtr timer{al_create_timer(1.0 / 60.0), al_destroy_timer};
    FontPtr font{al_create_builtin_font(), al_destroy_font};
    
    BitmapPtr background{al_load_bitmap("assets/start_screen_background.png"), al_destroy_bitmap};
    BitmapPtr mario{al_load_bitmap("assets/mario.png"), al_destroy_bitmap};
    
    if (background) al_convert_mask_to_alpha(background.get(), al_map_rgb(255, 0, 255));
    if (mario) al_convert_mask_to_alpha(mario.get(), al_map_rgb(255, 0, 255));

    auto menu_color = al_map_rgb(255, 255, 255);
    auto start_text = render_text_bitmap(font.get(), "Start", menu_color, true);
    auto options_text = render_text_bitmap(font.get(), "Options", menu_color, true);
    auto exit_text = render_text_bitmap(font.get(), "Exit", menu_color, true);
    auto pointer_text = render_text_bitmap(font.get(), ">", menu_color, true);

    EventQueuePtr queue{al_create_event_queue(), al_destroy_event_queue};
    al_register_event_source(queue.get(), al_get_display_event_source(display.get()));
    al_register_event_source(queue.get(), al_get_timer_event_source(timer.get()));
    al_register_event_source(queue.get(), al_get_keyboard_event_source());

    bool redraw = true;
    MenuChoice selected = MenuChoice::Start;
    const float scale = static_cast<float>(height) / 192.0f;
    float mario_x = (static_cast<float>(width) - 16.0f * scale) / 2.0f;
    const float ground_y = (static_cast<float>(height) * 0.52f) - (16.0f * scale) / 2.0f + 225.0f;
    float mario_y = ground_y;
    
    bool move_left = false, move_right = false, jumping = false;
    constexpr float move_speed = 3.5f, gravity = 0.6f, jump_speed = 12.0f;
    float jump_velocity = 0.0f;
    float anim_timer = 0.0f;
    int anim_index = 0, move_frame_hold = 0;
    bool facing_left = false, just_landed = false;

    al_start_timer(timer.get());

    bool running = true;
    while (running) {
        ALLEGRO_EVENT event;
        al_wait_for_event(queue.get(), &event);

        if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            selected = MenuChoice::Exit;
            running = false;
        } else if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
            switch (event.keyboard.keycode) {
                case ALLEGRO_KEY_ESCAPE: selected = MenuChoice::Exit; running = false; break;
                case ALLEGRO_KEY_O: if (mario_y >= ground_y) { jumping = true; jump_velocity = -jump_speed; } break;
                case ALLEGRO_KEY_A: move_left = true; anim_index = 1; move_frame_hold = 2; facing_left = true; redraw = true; break;
                case ALLEGRO_KEY_D: move_right = true; anim_index = 1; move_frame_hold = 2; facing_left = false; redraw = true; break;
                case ALLEGRO_KEY_TAB: 
                    if (selected == MenuChoice::Start) selected = MenuChoice::Options;
                    else if (selected == MenuChoice::Options) selected = MenuChoice::Exit;
                    else selected = MenuChoice::Start;
                    redraw = true;
                    break;
                case ALLEGRO_KEY_ENTER: running = false; break;
            }
        } else if (event.type == ALLEGRO_EVENT_KEY_UP) {
            switch (event.keyboard.keycode) {
                case ALLEGRO_KEY_A: move_left = false; if (!move_right && !jumping) { move_frame_hold = 0; anim_index = 0; } redraw = true; break;
                case ALLEGRO_KEY_D: move_right = false; if (!move_left && !jumping) { move_frame_hold = 0; anim_index = 0; } redraw = true; break;
            }
        } else if (event.type == ALLEGRO_EVENT_TIMER) {
            bool moved = false;
            if (move_left) { mario_x -= move_speed; moved = true; }
            if (move_right) { mario_x += move_speed; moved = true; }
            if (jumping) {
                mario_y += jump_velocity;
                jump_velocity += gravity;
                if (mario_y >= ground_y) {
                    mario_y = ground_y; jumping = false; jump_velocity = 0.0f; anim_index = 0; just_landed = true;
                }
                moved = true;
                redraw = true;
            }
            if (mario_x < 0) mario_x = 0;
            const float max_x = static_cast<float>(width) - 16.0f * scale;
            if (mario_x > max_x) mario_x = max_x;

            if (just_landed) { anim_timer = 0; move_frame_hold = 0; anim_index = 0; redraw = true; }
            else if (moved) {
                anim_timer += 0.15f;
                if (anim_timer >= 1.0f) {
                    anim_timer = 0;
                    anim_index = (anim_index + 1) % 4;
                    if (anim_index == 0) anim_index = 1;
                }
                if (jumping) anim_index = 0;
                move_frame_hold = 2;
                redraw = true;
            } else {
                if (move_frame_hold > 0) { move_frame_hold--; anim_index = 1; redraw = true; }
                else anim_index = 0;
            }
            just_landed = false;
        }

        if (redraw && al_is_event_queue_empty(queue.get())) {
            float mario_src_x = 0;
            if (jumping) mario_src_x = 65.0f;
            else {
                switch (anim_index) {
                    case 0: mario_src_x = 0; break;
                    case 1: mario_src_x = 17.0f; break;
                    case 2: mario_src_x = 33.0f; break;
                    case 3: mario_src_x = 49.0f; break;
                    default: mario_src_x = 0; break;
                }
            }
            draw_start_screen(background.get(), mario.get(), start_text.get(), options_text.get(), exit_text.get(), pointer_text.get(), width, height, font.get(), selected, mario_x, mario_y, mario_src_x, facing_left, scale);
            al_flip_display();
            redraw = false;
        }
    }

    return selected;
}

}
