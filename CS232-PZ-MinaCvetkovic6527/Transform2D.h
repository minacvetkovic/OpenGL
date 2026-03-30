#pragma once
#include "glm.hpp"
#include "glut.h"

struct Transform2D {
public:
    glm::vec2 position = glm::vec2(0.0f);
    float rotation_deg = 0.0f;
    glm::vec2 scale = glm::vec2(1.0f);
    glm::vec2 velocity;
};

inline void apply_transform(const Transform2D& transform) {
    glTranslatef(transform.position.x, transform.position.y, 0.0f);
    glRotatef(transform.rotation_deg, 0.0f, 0.0f, 1.0f);
    glScalef(transform.scale.x, transform.scale.y, 1.0f);
}