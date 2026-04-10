#pragma once

#include <atomic>
#include "../common/direction.h"

struct SharedState {
    // Mario state (read by Renderer and World)
    std::atomic<float> mario_x{0.0f};
    std::atomic<float> mario_y{0.0f};
    std::atomic<Direction> mario_direction{Direction::Right};
    std::atomic<bool> mario_is_walking{false};
    std::atomic<bool> mario_is_jumping{false};
    std::atomic<int> mario_current_frame{0};

    // World state (read by Renderer and Mario)
    std::atomic<float> world_scroll_x{0.0f};
    std::atomic<float> ground_y{0.0f};

    // Input state (updated by Engine, read by Mario)
    std::atomic<bool> key_forward{false};
    std::atomic<bool> key_backward{false};
    std::atomic<bool> key_jump{false};

    // Control
    std::atomic<bool> running{true};
};
