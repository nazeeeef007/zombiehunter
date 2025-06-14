#pragma once
#include "raylib.h"
#include <vector>
#include "raymath.h"

class Zombie {
public:
    enum class ZombieState {
        CHASING,
        ATTACKING,
        DYING, // Zombie is exploding/fading out
        DEAD   // Ready for removal from game
    };

    Vector2 pos;
    float speed;
    int health;
    int maxHealth;
    int damage;
    float size;
    Color bodyColor; // Base color for the zombie skin

    // --- State & Timer Variables ---
    ZombieState currentState;
    float attackCooldownTimer;
    float hitFlashTimer;
    float deathTimer; // Timer for the dying animation (explosion/fade)

    // Explosion specific properties
    float explosionRadius; // Current radius of the explosion effect
    float explosionAlpha;  // Current alpha of the explosion effect

    // --- Constants ---
    static const float ZOMBIE_ATTACK_COOLDOWN;
    static const float ZOMBIE_HIT_FLASH_DURATION;
    static const float ZOMBIE_DEATH_DURATION;       // How long the dying animation lasts
    static const float ZOMBIE_ATTACK_RANGE_BUFFER;
    static const float ZOMBIE_EXPLOSION_MAX_RADIUS; // Max size of the explosion particles/effect

    Zombie(Vector2 pos, float speed, int health, int damage, float size, Color color = GREEN);

    virtual void update(Vector2 playerPos, float deltaTime, const std::vector<Rectangle>& walls, float& playerHealth);
    virtual void draw() const;

    virtual void takeDamage(int dmg);
    Vector2 getPos() const { return pos; }
    float getSize() const { return size; }
    bool isDead() const { return currentState == ZombieState::DEAD; }

    virtual ~Zombie() = default;

private:
    void handleChasingState(Vector2 playerPos, float deltaTime, const std::vector<Rectangle>& walls);
    void handleAttackingState(Vector2 playerPos, float deltaTime, float& playerHealth);
    void handleDyingState(float deltaTime); // Updates explosion/fade
    void updateTimers(float deltaTime);

    // Helper to check if zombie at a given position collides with any wall
    bool checkWallCollision(Vector2 checkPos, float checkSize, const std::vector<Rectangle>& walls) const;
    // New: Resolves a single circle-rectangle collision by pushing the circle out
    void resolveSingleWallCollision(Vector2& circlePos, float circleRadius, const Rectangle& wall) const;

    void drawHealthBar() const;
    void drawExplosionEffect() const; // Draws the death explosion
};