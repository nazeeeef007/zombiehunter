// ZombieTypes.h
#pragma once
#include "Zombie.h"
#include "raylib.h"

class FastZombie : public Zombie {
public:
    FastZombie(Vector2 pos) : Zombie(pos, 120.0f, 50, 5, 15.0f, RED) {}
    // No draw override — use base draw() which respects bodyColor
};

class TankZombie : public Zombie {
public:
    TankZombie(Vector2 pos) : Zombie(pos, 40.0f, 200, 20, 25.0f, DARKGREEN) {}
    // No draw override
};
