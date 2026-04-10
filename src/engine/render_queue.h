#pragma once

#include <allegro5/allegro.h>
#include <vector>
#include <mutex>
#include <variant>

struct ClearCommand {
    ALLEGRO_COLOR color;
    int depth = 0;
};

struct DrawBitmapCommand {
    ALLEGRO_BITMAP* bitmap;
    float dx, dy;
    int depth = 10;
};

struct DrawScaledBitmapCommand {
    ALLEGRO_BITMAP* bitmap;
    float dx, dy, dw, dh;
    int depth = 10;
};

struct DrawScaledExCommand {
    ALLEGRO_BITMAP* bitmap;
    float sx, sy, sw, sh;
    float dx, dy, dw, dh;
    int flags;
    int depth = 10;
};

struct PresentCommand {
    int depth = 100;
};

using RenderCommand = std::variant<
    ClearCommand,
    DrawBitmapCommand,
    DrawScaledBitmapCommand,
    DrawScaledExCommand,
    PresentCommand
>;

class RenderQueue {
private:
    std::vector<RenderCommand> commands;
    std::mutex mutex;

public:
    void push(RenderCommand command) {
        std::lock_guard<std::mutex> lock(mutex);
        commands.push_back(command);
    }

    std::vector<RenderCommand> pop_all() {
        std::lock_guard<std::mutex> lock(mutex);
        std::vector<RenderCommand> result = std::move(commands);
        commands.clear();
        return result;
    }
};
