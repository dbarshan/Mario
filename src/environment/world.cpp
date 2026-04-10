#include "world.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>

namespace fs = std::filesystem;

World::World() : world_number(1), subworld_number(1), visible_width(256), visible_height(240) {
    world_name = std::to_string(world_number) + "-" + std::to_string(subworld_number);
    
    fs::path sprite_path = fs::path("assets") / ("world" + world_name) / "overworld.png";
    sprite = al_load_bitmap(sprite_path.string().c_str());
    if (!sprite) {
        std::cerr << "Failed to load world sprite: " << sprite_path << std::endl;
    }
    start_pos = Pos{0.0f, 0.0f};
}

World::~World() {
    if (sprite) al_destroy_bitmap(sprite);
}

void World::update_world(int world_number, int subworld_number) {
    this->world_number = world_number;
    this->subworld_number = subworld_number;
    world_name = std::to_string(world_number) + "-" + std::to_string(subworld_number);
    
    fs::path sprite_path = fs::path("assets") / ("world" + world_name) / "overworld.png";
    if (sprite) al_destroy_bitmap(sprite);
    sprite = al_load_bitmap(sprite_path.string().c_str());
    if (!sprite) {
        std::cerr << "Failed to load world sprite: " << sprite_path << std::endl;
    }
    start_pos = Pos{0.0f, 0.0f};
}

void World::scroll(float delta_x) {
    start_pos.x += delta_x;
    if (sprite) {
        float max_scroll = static_cast<float>(al_get_bitmap_width(sprite) - visible_width);
        if (start_pos.x > max_scroll) start_pos.x = max_scroll;
    }
    if (start_pos.x < 0.0f) start_pos.x = 0.0f;
}

void World::draw(Renderer* renderer) {
    if (!sprite) return;
    renderer->draw_bitmap_region_scaled(
        sprite,
        start_pos.x, start_pos.y,
        static_cast<float>(visible_width), static_cast<float>(visible_height),
        0.0f, 0.0f,
        static_cast<float>(renderer->width()), static_cast<float>(renderer->height())
    );
}

std::optional<World::TileMapData> World::read_tile_map() const {
    TileMapData result{0, 0, {}};
    fs::path tile_map_path = fs::path("assets") / ("world" + world_name) / "tile_map.bin";
    
    std::ifstream file(tile_map_path, std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open tile map: " << tile_map_path << std::endl;
        return std::nullopt;
    }

    if (!file.read(reinterpret_cast<char*>(&result.rows), 4) ||
        !file.read(reinterpret_cast<char*>(&result.cols), 4)) {
        std::cerr << "Failed to read tile map header" << std::endl;
        return std::nullopt;
    }

    result.matrix.resize(result.rows, std::vector<uint8_t>(result.cols));
    for (int r = 0; r < result.rows; ++r) {
        if (!file.read(reinterpret_cast<char*>(result.matrix[r].data()), result.cols)) {
            std::cerr << "Failed to read tile map data at row " << r << std::endl;
            return std::nullopt;
        }
    }

    return result;
}

Pos World::get_player_spawn_pos(float scale) {
    if (auto data_opt = read_tile_map(); data_opt) {
        auto& [rows, cols, matrix] = *data_opt;
        // Search for the first ground tile (0) starting from the beginning (column 2)
        for (int col = 2; col < cols; ++col) {
            for (int row = 0; row < rows; ++row) {
                if (matrix[row][col] == 0) {
                    // This is a ground tile. Mario should stand on the tile above it.
                    return Pos{static_cast<float>(col) * scale, static_cast<float>(row - 1) * scale};
                }
            }
        }
    }

    return Pos{0.0f, 0.0f};
}
