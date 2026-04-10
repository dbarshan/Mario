#include "renderer.h"

Renderer::Renderer(ALLEGRO_DISPLAY* display, RenderQueue* queue) 
    : display(display), queue(queue), w(0), h(0) {
    if (display) {
        w = al_get_display_width(display);
        h = al_get_display_height(display);
    }
}

Renderer::Renderer(int width, int height, RenderQueue* queue)
    : display(nullptr), queue(queue), w(width), h(height) {}

void Renderer::clear(ALLEGRO_COLOR color) {
    if (queue) {
        queue->push(ClearCommand{color});
    } else {
        al_clear_to_color(color);
    }
}

void Renderer::draw_text(ALLEGRO_FONT* font, ALLEGRO_COLOR color, float x, float y, int align, std::string_view text) {
    // Text drawing is a bit complex for variant/queue without allocation, 
    // for now we'll skip text in the queue or just draw it directly if on the right thread.
    // In a full implementation, we'd add DrawTextCommand.
    al_draw_text(font, color, x, y, align, text.data());
}

void Renderer::draw_bitmap_region(
    ALLEGRO_BITMAP* sprite,
    float sx, float sy, float sw, float sh,
    float dx, float dy
) {
    if (queue) {
        queue->push(DrawScaledExCommand{sprite, sx, sy, sw, sh, dx, dy, sw, sh, 0});
    } else {
        al_draw_bitmap_region(sprite, sx, sy, sw, sh, dx, dy, 0);
    }
}

void Renderer::draw_bitmap_region_scaled(
    ALLEGRO_BITMAP* sprite,
    float sx, float sy, float sw, float sh,
    float dx, float dy, float dw, float dh
) {
    if (queue) {
        queue->push(DrawScaledExCommand{sprite, sx, sy, sw, sh, dx, dy, dw, dh, 0});
    } else {
        al_draw_scaled_bitmap(sprite, sx, sy, sw, sh, dx, dy, dw, dh, 0);
    }
}

void Renderer::draw_bitmap(ALLEGRO_BITMAP* sprite, float dx, float dy) {
    if (queue) {
        queue->push(DrawBitmapCommand{sprite, dx, dy});
    } else {
        al_draw_bitmap(sprite, dx, dy, 0);
    }
}

void Renderer::draw_bitmap_scaled(ALLEGRO_BITMAP* sprite, float dx, float dy, float dw, float dh) {
    if (queue) {
        queue->push(DrawScaledBitmapCommand{sprite, dx, dy, dw, dh});
    } else {
        float sw = static_cast<float>(al_get_bitmap_width(sprite));
        float sh = static_cast<float>(al_get_bitmap_height(sprite));
        al_draw_scaled_bitmap(sprite, 0, 0, sw, sh, dx, dy, dw, dh, 0);
    }
}

void Renderer::draw_scaled_bitmap_ex(
    ALLEGRO_BITMAP* sprite,
    float sx, float sy, float sw, float sh,
    float dx, float dy, float dw, float dh,
    int flags
) {
    if (queue) {
        queue->push(DrawScaledExCommand{sprite, sx, sy, sw, sh, dx, dy, dw, dh, flags});
    } else {
        al_draw_scaled_bitmap(sprite, sx, sy, sw, sh, dx, dy, dw, dh, flags);
    }
}

int Renderer::width() const {
    return w;
}

int Renderer::height() const {
    return h;
}

void Renderer::present() {
    if (queue) {
        queue->push(PresentCommand{});
    } else {
        al_flip_display();
    }
}
