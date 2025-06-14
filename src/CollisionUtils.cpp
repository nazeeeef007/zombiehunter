#include "raylib.h"
#include <algorithm>
#include "raymath.h"
#include <vector>

//CollisionUtils.cpp
bool CollidesWithWall(Vector2 point, const std::vector<Rectangle>& walls) {
    for (const auto& wall : walls) {
        if (CheckCollisionPointRec(point, wall)) return true;
    }
    return false;
}

bool CollidesWithWallCircle(Vector2 center, float radius, const std::vector<Rectangle>& walls) {
    for (const auto& wall : walls) {
        if (CheckCollisionCircleRec(center, radius, wall)) return true;
    }
    return false;
}