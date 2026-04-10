#include "options_screen.h"
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <vector>
#include <string_view>
#include <memory>
#include <array>

namespace OptionsScreen {

using DisplayPtr = std::unique_ptr<ALLEGRO_DISPLAY, void(*)(ALLEGRO_DISPLAY*)>;
using TimerPtr = std::unique_ptr<ALLEGRO_TIMER, void(*)(ALLEGRO_TIMER*)>;
using FontPtr = std::unique_ptr<ALLEGRO_FONT, void(*)(ALLEGRO_FONT*)>;
using EventQueuePtr = std::unique_ptr<ALLEGRO_EVENT_QUEUE, void(*)(ALLEGRO_EVENT_QUEUE*)>;

void draw_options(ALLEGRO_FONT* font, int width, int height, int selected) {
    al_clear_to_color(al_map_rgb(0, 0, 0));

    constexpr float title_y = 80.0f;
    al_draw_text(font, al_map_rgb(255, 255, 255), width / 2, title_y, ALLEGRO_ALIGN_CENTRE, "Options");

    static constexpr std::array<std::string_view, 5> items = {"Video", "Sound", "Controls", "About", "Back"};
    constexpr float start_y = 200.0f;
    constexpr float gap = 50.0f;
    
    for (std::size_t i = 0; i < items.size(); ++i) {
        float y = start_y + gap * static_cast<float>(i);
        al_draw_text(font, al_map_rgb(255, 255, 255), width / 2, y, ALLEGRO_ALIGN_CENTRE, items[i].data());
        if (i == static_cast<std::size_t>(selected)) {
            al_draw_text(font, al_map_rgb(255, 255, 255), width / 2 - 80, y, ALLEGRO_ALIGN_RIGHT, ">");
        }
    }

    al_draw_text(font, al_map_rgb(160, 160, 160), width / 2, static_cast<float>(height) - 60.0f, ALLEGRO_ALIGN_CENTRE, "Press Esc to return");
}

void run() {
    constexpr int width = 1024;
    constexpr int height = 768;
    
    DisplayPtr display{al_create_display(width, height), al_destroy_display};
    TimerPtr timer{al_create_timer(1.0 / 60.0), al_destroy_timer};
    FontPtr font{al_create_builtin_font(), al_destroy_font};
    EventQueuePtr queue{al_create_event_queue(), al_destroy_event_queue};

    if (display) al_register_event_source(queue.get(), al_get_display_event_source(display.get()));
    if (timer) al_register_event_source(queue.get(), al_get_timer_event_source(timer.get()));
    al_register_event_source(queue.get(), al_get_keyboard_event_source());

    bool redraw = true;
    int selected = 0;
    if (timer) al_start_timer(timer.get());

    bool running = true;
    while (running) {
        ALLEGRO_EVENT event;
        al_wait_for_event(queue.get(), &event);

        if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            running = false;
        } else if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
            switch (event.keyboard.keycode) {
                case ALLEGRO_KEY_ESCAPE: running = false; break;
                case ALLEGRO_KEY_TAB: selected = (selected + 1) % 5; redraw = true; break;
                case ALLEGRO_KEY_ENTER: if (selected == 4) running = false; break;
                default: break;
            }
        } else if (event.type == ALLEGRO_EVENT_TIMER) {
            redraw = true;
        }

        if (redraw && al_is_event_queue_empty(queue.get())) {
            draw_options(font.get(), width, height, selected);
            al_flip_display();
            redraw = false;
        }
    }
}

}
