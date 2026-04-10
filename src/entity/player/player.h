#pragma once

#include "../entity.h"

class Player : public Entity {
public:
    virtual void stand() = 0;
    virtual void walk() = 0;
    virtual void jump() = 0;
    virtual void grow() = 0;
    virtual void shrink() = 0;
};