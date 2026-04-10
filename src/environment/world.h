#pragma once

#include <allegro5/allegro.h>
#include <string>
#include <vector>
#include <cstdint>
#include <optional>
#include <filesystem>
#include "../common/pos.h"
#include "../renderer/renderer.h"

class World {
public:
    int world_number;
    int subworld_number;
    std::string world_name;
    ALLEGRO_BITMAP* sprite;
    Pos start_pos;
    int visible_width;
    int visible_height;

    World();
    ~World();

    void update_world(int world_number, int subworld_number);
    void scroll(float delta_x);
    void draw(Renderer* renderer);
    Pos get_player_spawn_pos(float scale);

private:
    struct TileMapData {
        int rows;
        int cols;
        std::vector<std::vector<uint8_t>> matrix;
    };

    [[nodiscard]] std::optional<TileMapData> read_tile_map() const;
};