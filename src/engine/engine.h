#pragma once

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <functional>
#include <thread>
#include <atomic>
#include "../renderer/renderer.h"
#include "shared_state.h"
#include "render_queue.h"

class Engine {
public:
    ALLEGRO_DISPLAY* display;
    int width;
    int height;
    SharedState shared_state;
    RenderQueue render_queue;

    Engine();
    ~Engine();

    void create_display(int width, int height);
    void start_main_loop(std::function<void(ALLEGRO_DISPLAY*)> tick);

private:
    void mario_thread_func();
    void world_thread_func(float scale);
    void logic_thread_func();
};