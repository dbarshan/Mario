#include "engine.h"
#include <iostream>
#include <memory>
#include <algorithm>
#include <chrono>
#include "../control/keyboard.h"
#include "../environment/world.h"
#include "../entity/player/mario.h"
#include "../renderer/renderer.h"

Engine::Engine() : display(nullptr), width(0), height(0) {
    if (!al_init()) {
        std::cerr << "Failed to initialize Allegro" << std::endl;
        return;
    }
    if (!al_init_image_addon()) {
        std::cerr << "Failed to initialize image addon" << std::endl;
        return;
    }
    al_install_keyboard();
}

Engine::~Engine() {
    shared_state.running = false;
    if (display) {
        al_destroy_display(display);
    }
}

void Engine::create_display(int width, int height) {
    al_set_new_display_flags(ALLEGRO_WINDOWED | ALLEGRO_RESIZABLE);
    if (display = al_create_display(width, height); !display) {
        std::cerr << "Failed to create display" << std::endl;
        return;
    }
    this->width = width;
    this->height = height;
}

void Engine::mario_thread_func() {
    float start_x = shared_state.mario_x;
    float start_y = shared_state.mario_y;
    Mario mario(Pos{start_x, start_y});
    
    while (shared_state.running) {
        // Read input and world state from shared state
        bool left = shared_state.key_backward;
        bool right = shared_state.key_forward;
        bool jump = shared_state.key_jump;
        mario.ground_y = shared_state.ground_y;

        float speed = mario.is_jumping ? Mario::JUMP_H_SPEED : Mario::WALK_SPEED;
        bool moving = false;

        if (left) {
            mario.pos.x -= speed;
            mario.direction = Direction::Left;
            moving = true;
        }
        if (right) {
            mario.pos.x += speed;
            mario.direction = Direction::Right;
            moving = true;
        }

        if (moving) {
            mario.walk();
        } else {
            mario.stand();
        }

        if (jump) {
            mario.jump();
        }

        mario.update_physics();

        // Update shared state for the main render thread
        shared_state.mario_x = mario.pos.x;
        shared_state.mario_y = mario.pos.y;
        shared_state.mario_direction = mario.direction;
        shared_state.mario_is_walking = mario.is_walking;
        shared_state.mario_is_jumping = mario.is_jumping;
        shared_state.mario_current_frame = mario.current_frame;

        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
}

void Engine::world_thread_func(float scale) {
    World world;
    
    // Initial spawn pos setup
    float scale_factor = scale * 16.0f; // DEFAULT_SPRITE_SIZE
    if (auto spawn_pos = world.get_player_spawn_pos(scale_factor); spawn_pos.x >= 0.0f && spawn_pos.y >= 0.0f) {
        shared_state.mario_x = spawn_pos.x;
        shared_state.mario_y = spawn_pos.y;
        shared_state.ground_y = spawn_pos.y;
    }

    while (shared_state.running) {
        float mario_x = shared_state.mario_x;
        float scroll_threshold = static_cast<float>(width) * 0.5f;

        if (mario_x > scroll_threshold) {
            float diff = mario_x - scroll_threshold;
            float scroll_dx = diff * (static_cast<float>(world.visible_width) / static_cast<float>(width));
            world.scroll(scroll_dx);
            
            shared_state.world_scroll_x = world.start_pos.x;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
}

void Engine::logic_thread_func() {
    while (shared_state.running) {
        // High level logic like checking for collisions between entities
        // or game over conditions.
        
        // For now, just a heartbeat
        std::this_thread::sleep_for(std::chrono::milliseconds(32));
    }
}

void Engine::start_main_loop(std::function<void(ALLEGRO_DISPLAY*)> tick) {
    if (!display) return;

    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    al_register_event_source(queue, al_get_display_event_source(display));
    al_register_event_source(queue, al_get_keyboard_event_source());

    float scale = static_cast<float>(height) / 240.0f;

    // Start threads for logic
    std::thread world_thread(&Engine::world_thread_func, this, scale);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::thread mario_thread(&Engine::mario_thread_func, this);
    std::thread logic_thread(&Engine::logic_thread_func, this);

    // Main thread handles ALL rendering to prevent flickering
    Renderer main_renderer(display);
    World render_world;
    Mario render_mario(Pos{0, 0});

    while (shared_state.running) {
        ALLEGRO_EVENT event;
        while (al_get_next_event(queue, &event)) {
            if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
                shared_state.running = false;
            } else if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
                if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE) shared_state.running = false;
                if (event.keyboard.keycode == ALLEGRO_KEY_D || event.keyboard.keycode == ALLEGRO_KEY_RIGHT) shared_state.key_forward = true;
                if (event.keyboard.keycode == ALLEGRO_KEY_A || event.keyboard.keycode == ALLEGRO_KEY_LEFT) shared_state.key_backward = true;
                if (event.keyboard.keycode == ALLEGRO_KEY_O || event.keyboard.keycode == ALLEGRO_KEY_SPACE || event.keyboard.keycode == ALLEGRO_KEY_W || event.keyboard.keycode == ALLEGRO_KEY_UP) shared_state.key_jump = true;
            } else if (event.type == ALLEGRO_EVENT_KEY_UP) {
                if (event.keyboard.keycode == ALLEGRO_KEY_D || event.keyboard.keycode == ALLEGRO_KEY_RIGHT) shared_state.key_forward = false;
                if (event.keyboard.keycode == ALLEGRO_KEY_A || event.keyboard.keycode == ALLEGRO_KEY_LEFT) shared_state.key_backward = false;
                if (event.keyboard.keycode == ALLEGRO_KEY_O || event.keyboard.keycode == ALLEGRO_KEY_SPACE || event.keyboard.keycode == ALLEGRO_KEY_W || event.keyboard.keycode == ALLEGRO_KEY_UP) shared_state.key_jump = false;
            }
        }

        // 1. Update render objects from shared state
        render_world.start_pos.x = shared_state.world_scroll_x;
        render_mario.pos.x = shared_state.mario_x;
        render_mario.pos.y = shared_state.mario_y;
        render_mario.direction = shared_state.mario_direction;
        render_mario.is_walking = shared_state.mario_is_walking;
        render_mario.is_jumping = shared_state.mario_is_jumping;
        render_mario.current_frame = shared_state.mario_current_frame;

        // 2. Render complete frame
        main_renderer.clear(al_map_rgb(107, 140, 255)); // Sky blue background
        render_world.draw(&main_renderer);
        render_mario.draw(&main_renderer);

        al_flip_display();

        if (tick) tick(display);

        std::this_thread::sleep_for(std::chrono::milliseconds(16)); // Target ~60 FPS
    }

    world_thread.join();
    mario_thread.join();
    logic_thread.join();
    
    al_destroy_event_queue(queue);
}
