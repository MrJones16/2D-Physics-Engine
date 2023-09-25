#pragma once
#include <vector>
#include "vectors.hpp"


struct VerletObject{

    Vec2 position_current;
    Vec2 position_previous;
    Vec2 acceleration;
    float size;
    uint32_t color;

    VerletObject();
    VerletObject(float xpos, float ypos);
    VerletObject(float xpos, float ypos, float size);

    void UpdatePosition(float dt);
    void Accelerate(Vec2 acc);
    void Accelerate(float x, float y);
};