#include "mario.h"
#include <iostream>

Mario::Mario(Pos pos) : pos(pos), direction(Direction::Right), state{}, sprite_size{DEFAULT_SPRITE_SIZE}, 
    current_frame{0}, frame_timer{0}, is_walking{false}, anim_direction{1}, 
    is_jumping{false}, vel_y{0.0f}, ground_y{pos.y} {
    
    if (auto sheet = al_load_bitmap("assets/mario.png"); sheet) {
        al_convert_mask_to_alpha(sheet, al_map_rgb(255, 0, 255));

        standing_sprite = al_create_sub_bitmap(sheet, 0, 0, 16, 16);
        
        walking_sprites.push_back(al_create_sub_bitmap(sheet, 16, 0, 16, 16));
        walking_sprites.push_back(al_create_sub_bitmap(sheet, 32, 0, 16, 16));
        walking_sprites.push_back(al_create_sub_bitmap(sheet, 48, 0, 16, 16));

        jumping_sprite = al_create_sub_bitmap(sheet, 64, 0, 16, 16);
        
        // Note: Allegro 5 sub-bitmaps depend on the parent sheet.
        // We should ideally store the sheet as well to ensure it stays alive.
        // For this simple conversion, we'll assume bitmaps are managed.
    } else {
        std::cerr << "Failed to load mario sprite sheet" << std::endl;
        standing_sprite = nullptr;
        jumping_sprite = nullptr;
    }
}

Mario::~Mario() {
    if (standing_sprite) al_destroy_bitmap(standing_sprite);
    for (auto* s : walking_sprites) {
        if (s) al_destroy_bitmap(s);
    }
    if (jumping_sprite) al_destroy_bitmap(jumping_sprite);
}

void Mario::stand() {
    is_walking = false;
    current_frame = 0;
    frame_timer = 0;
    anim_direction = 1;
}

void Mario::walk() {
    is_walking = true;
    frame_timer++;
    if (const auto walk_size = static_cast<int>(walking_sprites.size()); walk_size > 0 && frame_timer >= 10) {
        frame_timer = 0;
        int next_frame = current_frame + anim_direction;
        if (next_frame >= walk_size) {
            current_frame = walk_size - 2;
            anim_direction = -1;
        } else if (next_frame < 0) {
            current_frame = 1;
            anim_direction = 1;
        } else {
            current_frame = next_frame;
        }
    } else if (walking_sprites.size() == 1) {
        current_frame = 0;
    }
}

void Mario::jump() {
    if (!is_jumping) {
        is_jumping = true;
        vel_y = JUMP_VELOCITY;
    }
}

void Mario::spawn(Renderer* renderer, Pos pos) {
    this->pos = pos;
    this->ground_y = pos.y;
    draw(renderer);
}

void Mario::draw(Renderer* renderer) {
    if (!renderer) return;

    const float scale = static_cast<float>(renderer->height()) / 240.0f;
    
    ALLEGRO_BITMAP* sprite = nullptr;
    if (is_jumping) {
        sprite = jumping_sprite;
    } else if (is_walking && !walking_sprites.empty()) {
        sprite = walking_sprites[static_cast<std::size_t>(current_frame) % walking_sprites.size()];
    } else {
        sprite = standing_sprite;
    }

    if (!sprite) return;

    const float dw = static_cast<float>(al_get_bitmap_width(sprite)) * scale;
    const float dh = static_cast<float>(al_get_bitmap_height(sprite)) * scale;
    
    const int flags = (direction == Direction::Left) ? ALLEGRO_FLIP_HORIZONTAL : 0;

    renderer->draw_scaled_bitmap_ex(
        sprite,
        0, 0, static_cast<float>(al_get_bitmap_width(sprite)), static_cast<float>(al_get_bitmap_height(sprite)),
        pos.x, pos.y, dw, dh,
        flags
    );
}

void Mario::update_physics() {
    // If Mario is above the ground, apply gravity
    if (is_jumping || pos.y < ground_y) {
        vel_y += GRAVITY;
        pos.y += vel_y;

        // Check if Mario has hit or passed the ground
        if (pos.y >= ground_y) {
            pos.y = ground_y;
            vel_y = 0.0f;
            is_jumping = false;
        }
    } else if (pos.y > ground_y) {
        // Snap to ground if somehow below it
        pos.y = ground_y;
        vel_y = 0.0f;
    }
}
