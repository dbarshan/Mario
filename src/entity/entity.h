#pragma once

#include "../common/pos.h"
#include "../renderer/renderer.h"

class Entity {
public:
    virtual ~Entity() {}
    virtual void spawn(Renderer* renderer, Pos pos) = 0;
    virtual void die() = 0;
};