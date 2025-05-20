#pragma once
#include <vector>
#include "raylib.h"
#include "Weapon.h"
#include "Bullet.h"
#include "Zombie.h"  // Add this to use Zombie class

class Player {
public:
    Vector2 pos;
    Vector2 facing;
    float size;
    float health;

    Weapon weapon;
    std::vector<Bullet> bullets;

    Player(Vector2 startPos, Vector2 startFacing, float size, int health, Weapon weapon);

    void update(float deltaTime, std::vector<Zombie>& zombies, const std::vector<Rectangle>& walls);

    void draw() const;

    void shoot(float currentTime);
};
