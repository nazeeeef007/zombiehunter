#pragma once
#include "raylib.h"
#include <vector>

bool CollidesWithWall(Vector2 point, const std::vector<Rectangle>& walls);
bool CollidesWithWallCircle(Vector2 center, float radius, const std::vector<Rectangle>& walls);
