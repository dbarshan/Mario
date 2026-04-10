#pragma once

#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <string_view>
#include <optional>
#include "../engine/render_queue.h"

class Renderer {
private:
    ALLEGRO_DISPLAY* display;
    RenderQueue* queue;
    int w;
    int h;

public:
    explicit Renderer(ALLEGRO_DISPLAY* display, RenderQueue* queue = nullptr);
    Renderer(int width, int height, RenderQueue* queue);

    void clear(ALLEGRO_COLOR color);

    void draw_text(ALLEGRO_FONT* font, ALLEGRO_COLOR color, float x, float y, int align, std::string_view text);

    void draw_bitmap_region(
        ALLEGRO_BITMAP* sprite,
        float sx, float sy, float sw, float sh,
        float dx, float dy
    );

    void draw_bitmap_region_scaled(
        ALLEGRO_BITMAP* sprite,
        float sx, float sy, float sw, float sh,
        float dx, float dy, float dw, float dh
    );

    void draw_bitmap(ALLEGRO_BITMAP* sprite, float dx, float dy);

    void draw_bitmap_scaled(ALLEGRO_BITMAP* sprite, float dx, float dy, float dw, float dh);

    void draw_scaled_bitmap_ex(
        ALLEGRO_BITMAP* sprite,
        float sx, float sy, float sw, float sh,
        float dx, float dy, float dw, float dh,
        int flags
    );

    [[nodiscard]] int width() const;
    [[nodiscard]] int height() const;

    void present();
};