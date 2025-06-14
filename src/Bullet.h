#pragma once
#include "raylib.h"

class Bullet {
public:
    Vector2 pos;
    Vector2 velocity;
    int damage;
    float radius;
    bool active;

    Bullet(Vector2 pos, Vector2 velocity, int damage)
        : pos(pos), velocity(velocity), damage(damage), radius(3.0f), active(true) {}

    void update(float deltaTime) {
        pos.x += velocity.x * deltaTime;
        pos.y += velocity.y * deltaTime;
    }

    void draw() const {
        // Outer glow (larger, semi-transparent circle)
        Color glowColor = { 255, 50, 50, 100 }; // soft red glow
        float glowRadius = radius * 2.5f;
        DrawCircleV(pos, glowRadius, glowColor);

        // Core bullet (bright center)
        Color coreColor = RED;
        DrawCircleV(pos, radius, coreColor);

        // Motion trail (a small fading circle behind bullet)
        Vector2 trailPos = { pos.x - velocity.x * 0.05f, pos.y - velocity.y * 0.05f };
        Color trailColor = { 255, 80, 80, 150 };
        DrawCircleV(trailPos, radius * 0.8f, trailColor);

        // Optional: Add a tiny white sparkle at the front
        Vector2 sparklePos = { pos.x + velocity.x * 0.1f, pos.y + velocity.y * 0.1f };
        DrawCircleV(sparklePos, radius * 0.4f, WHITE);
    }
};
