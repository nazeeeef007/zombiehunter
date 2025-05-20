#include "Player.h"
#include <algorithm>
#include "raymath.h"



Player::Player(Vector2 startPos, Vector2 startFacing, float size, int health, Weapon weapon)
    : pos(startPos), facing(startFacing), size(size), health(health), weapon(weapon) {}

void Player::update(float deltaTime, std::vector<Zombie>& zombies, const std::vector<Rectangle>& walls) {
    // Handle zombie-player collision
    for (auto& zombie : zombies) {
        float dist = Vector2Distance(pos, zombie.getPos());
        if (dist < size + zombie.getSize()) {
            health -= 20 * deltaTime; // Zombie deals damage over time
        }
    }

    // Update bullets
    for (auto& bullet : bullets) {
        if (!bullet.active) continue;

        bullet.update(deltaTime);

        // Bullet-zombie collision
        for (auto& zombie : zombies) {
            if (!bullet.active) break;
            float dist = Vector2Distance(bullet.pos, zombie.getPos());
            if (dist < bullet.radius + zombie.getSize()) {
                zombie.takeDamage(bullet.damage);
                bullet.active = false;
                break;
            }
        }

        // Bullet-wall collision
        if (bullet.active) {
            for (const auto& wall : walls) {
                if (CheckCollisionPointRec(bullet.pos, wall)) {
                    bullet.active = false;
                    break;
                }
            }
        }

        // Boundary check
        if (bullet.active && (
            bullet.pos.x < 0 || bullet.pos.y < 0 ||
            bullet.pos.x > GetScreenWidth() || bullet.pos.y > GetScreenHeight())) {
            bullet.active = false;
        }
    }

    // Remove inactive bullets
    bullets.erase(std::remove_if(bullets.begin(), bullets.end(),
        [](const Bullet& b) { return !b.active; }), bullets.end());
}


void Player::draw() const {
    // Draw head
    float headRadius = size * 0.5f;
    Vector2 headCenter = { pos.x, pos.y - size }; // head above body
    DrawCircleV(headCenter, headRadius, LIGHTGRAY);

    // Draw body (vertical line)
    Vector2 neck = { pos.x, pos.y - size + headRadius * 2 };
    Vector2 hip = { pos.x, pos.y + size * 0.5f };
    DrawLineV(neck, hip, GRAY);

    // Arms
    Vector2 rightShoulder = { pos.x, pos.y - size * 0.3f };
    Vector2 rightHand = Vector2Add(rightShoulder, Vector2Scale(Vector2Normalize(facing), size * 1.2f));
    DrawLineV(rightShoulder, rightHand, GRAY);

    // Left arm remains out
    Vector2 leftHand = { pos.x - size, pos.y };
    DrawLineV(rightShoulder, leftHand, GRAY);

    // Legs
    Vector2 leftFoot = { pos.x - size * 0.5f, pos.y + size };
    Vector2 rightFoot = { pos.x + size * 0.5f, pos.y + size };
    DrawLineV(hip, leftFoot, GRAY);
    DrawLineV(hip, rightFoot, GRAY);

    weapon.draw(pos, facing);

    // Draw facing direction
    // Vector2 facingLine = Vector2Add(pos, Vector2Scale(Vector2Normalize(facing), size * 1.5f));
    // DrawLineV(pos, facingLine, RED);

    // Draw health bar
    float barWidth = 40;
    float barHeight = 5;
    float hpPercent = Clamp(health / 100.0f, 0.0f, 1.0f);
    DrawRectangle(pos.x - barWidth / 2, pos.y - size * 1.8f, barWidth, barHeight, DARKGRAY);
    DrawRectangle(pos.x - barWidth / 2, pos.y - size * 1.8f, barWidth * hpPercent, barHeight, GREEN);

    // Draw bullets
    for (const auto& bullet : bullets) {
        if (bullet.active) bullet.draw();
    }
}


void Player::shoot(float currentTime) {
    if (weapon.canFire(currentTime)) {
        Vector2 bulletVel = Vector2Normalize(facing);
        bulletVel.x *= weapon.bulletSpeed;
        bulletVel.y *= weapon.bulletSpeed;

        bullets.emplace_back(pos, bulletVel, weapon.damage);
    }
}


