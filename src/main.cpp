#include "engine/engine.h"

int main(int argc, char **argv) {
    Engine engine;
    engine.create_display(640, 480);
    engine.start_main_loop([](ALLEGRO_DISPLAY* display) {
        // Main game loop placeholder.
    });
    return 0;
}
