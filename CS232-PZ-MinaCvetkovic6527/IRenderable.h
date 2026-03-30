#pragma once
#include "Transform2D.h"

class IRenderable {
public:
    virtual ~IRenderable() = default;
    virtual void render() = 0;
    virtual void update(float delta_time) = 0;
};