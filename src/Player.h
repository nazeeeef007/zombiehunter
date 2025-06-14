#pragma once
#include <vector>
#include "raylib.h"
#include "Weapon.h"
#include "Bullet.h"
#include "Zombie.h" // Add this to use Zombie class
#include "raymath.h" // Needed for Vector2 operations in the header

class Player {
public:
    Vector2 pos;
    Vector2 facing;
    float size;
    float health;
    float maxHealth; 

    Weapon weapon;
    std::vector<Bullet> bullets;

    // --- New Features & State Variables (All public as requested) ---
    float invulnerabilityTimer; // Time player is invulnerable after taking damage
    float muzzleFlashTimer;     // Timer for muzzle flash effect
    float damageTakenFlashTimer; // Timer for flashing red after taking damage

    // Dash Ability
    bool isDashing;
    float dashTimer;
    Vector2 dashDirection; // Direction of the current dash
    
    // Health Regeneration
    float timeSinceLastDamage; // Tracks time since last damage for regen
    float regenTimer;          // Internal timer for regen ticks

    // --- Constants (static const float for class-wide constants) ---
    static const float INVULNERABILITY_DURATION;
    static const float MUZZLE_FLASH_DURATION;
    static const float DASH_DURATION;
    static const float DASH_SPEED_MULTIPLIER;
    static const float REGEN_COOLDOWN; // Time before regen starts after damage
    static const float REGEN_INTERVAL; // How often regen ticks
    static const float REGEN_AMOUNT;   // Health regained per tick
    static const float DAMAGE_FLASH_DURATION; // How long player flashes red after hit

    Player(Vector2 startPos, Vector2 startFacing, float size, int health, Weapon && weapon);

    void update(float deltaTime, std::vector<Zombie>& zombies, const std::vector<Rectangle>& walls);
    void draw() const; // Made const correctly
    void shoot(float currentTime);

    // New: centralized damage taking function
    void takeDamage(float amount);

    // New: Initiates a dash
    void dash(Vector2 direction);

private:
    // --- Private Helper Functions (for internal logic and drawing details) ---
    void updateInvulnerability(float deltaTime);
    void updateMuzzleFlash(float deltaTime);
    void updateDamageFlash(float deltaTime);
    void updateDash(float deltaTime); // Handles dash movement and timer
    void updateHealthRegen(float deltaTime); // Handles health regeneration

    // Drawing helpers
    void drawStickman() const;
    void drawHealthBar() const; // The health bar above player, not the HUD one
    void drawMuzzleFlash() const;
    void drawDashTrail() const; // Optional: For visual flair during dash
};