#pragma once
#include "raylib.h"
#include <vector>

class Zombie {
protected:
    Color bodyColor; // color to use for drawing

public:
    Vector2 pos;
    float speed;
    int health;
    int damage;
    float size;

    Zombie(Vector2 pos, float speed, int health, int damage, float size, Color color = GREEN)
        : pos(pos), speed(speed), health(health), damage(damage), size(size), bodyColor(color) {}

    virtual void update(Vector2 playerPos, float deltaTime, const std::vector<Rectangle>& walls, float& playerHealth);
    virtual void draw() const;
    virtual void takeDamage(int dmg);
    virtual bool isDead() const { return health <= 0; }

    Vector2 getPos() const { return pos; }
    float getSize() const { return size; }

    virtual ~Zombie() {}
};
