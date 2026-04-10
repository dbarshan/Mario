#pragma once

#include "player.h"
#include "../../common/direction.h"
#include "../../common/player_state.h"
#include <vector>
#include <allegro5/allegro.h>

class Mario : public Player {
public:
    inline static constexpr int DEFAULT_SPRITE_SIZE = 16;
    inline static constexpr float JUMP_VELOCITY = -10.0f;
    inline static constexpr float GRAVITY = 0.4f;
    inline static constexpr float WALK_SPEED = 3.5f;
    inline static constexpr float JUMP_H_SPEED = 4.5f;

    Pos pos;
    Direction direction;
    PlayerState state;
    ALLEGRO_BITMAP* standing_sprite;
    std::vector<ALLEGRO_BITMAP*> walking_sprites;
    ALLEGRO_BITMAP* jumping_sprite;
    int sprite_size;
    int current_frame;
    int frame_timer;
    bool is_walking;
    int anim_direction;
    bool is_jumping;
    float vel_y;
    float ground_y;

    explicit Mario(Pos pos);
    virtual ~Mario();

    void stand() override;
    void walk() override;
    void jump() override;
    void grow() override {}
    void shrink() override {}

    void spawn(Renderer* renderer, Pos pos) override;
    void die() override {}

    void draw(Renderer* renderer);
    void update_physics();
    [[nodiscard]] int get_player_sprite_size() const { return sprite_size; }
};