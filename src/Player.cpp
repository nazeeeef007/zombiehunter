#include "Player.h"
#include <algorithm> // For std::remove_if
#include "raymath.h"
#include "CollisionUtils.h" // Assuming CollisionUtils.h exists and has CollidesWithWallCircle
#include <utility> // For std::move

// Helper for clamping (if Raylib's Clamp macro is not globally available)
#ifndef CLAMP
#define CLAMP(value, min, max) ((value < min) ? min : (value > max) ? max : value)
#endif

// --- Static Constant Definitions ---
const float Player::INVULNERABILITY_DURATION = 0.5f;
const float Player::MUZZLE_FLASH_DURATION = 0.05f;
const float Player::DASH_DURATION = 0.15f; // Short burst
const float Player::DASH_SPEED_MULTIPLIER = 5.0f; // 5x normal speed during dash
const float Player::REGEN_COOLDOWN = 3.0f; // 3 seconds after last damage to start regen
const float Player::REGEN_INTERVAL = 0.5f; // Regen tick every 0.5 seconds
const float Player::REGEN_AMOUNT = 2.0f; // Regain 2 health per tick
const float Player::DAMAGE_FLASH_DURATION = 0.1f; // Player flashes red for 0.1 seconds

// Player constructor: Takes Weapon by rvalue reference and moves it
Player::Player(Vector2 startPos, Vector2 startFacing, float size, int health, Weapon&& weapon)
    : pos(startPos), facing(startFacing), size(size), health(static_cast<float>(health)),
      maxHealth(static_cast<float>(health)), weapon(std::move(weapon)), // IMPORTANT: Use std::move here
      invulnerabilityTimer(0.0f), muzzleFlashTimer(0.0f), damageTakenFlashTimer(0.0f),
      isDashing(false), dashTimer(0.0f), dashDirection({0,0}),
      timeSinceLastDamage(0.0f), regenTimer(0.0f) {
    
    // Ensure facing is normalized
    if (Vector2Length(facing) == 0) {
        this->facing = {1, 0}; // Default facing right
    } else {
        this->facing = Vector2Normalize(facing);
    }
}

// --- Public Update & Draw ---

void Player::update(float deltaTime, std::vector<Zombie>& zombies, const std::vector<Rectangle>& walls) {
    // Update all timers for various effects
    updateInvulnerability(deltaTime);
    updateMuzzleFlash(deltaTime);
    updateDamageFlash(deltaTime); // New: for hit flash
    updateDash(deltaTime);       // New: handles dash movement and state
    updateHealthRegen(deltaTime); // New: handles passive health regeneration

    // Handle zombie-player collision and damage
    // Player::takeDamage handles invulnerability
    for (auto& zombie : zombies) {
        float dist = Vector2Distance(pos, zombie.getPos());
        if (dist < size + zombie.getSize()) {
            takeDamage(20.0f * deltaTime); // Zombie deals damage over time
        }
    }

    // Update bullets (existing logic)
    for (size_t i = 0; i < bullets.size(); ++i) {
        Bullet& bullet = bullets[i];
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

    // Remove inactive bullets efficiently
    bullets.erase(std::remove_if(bullets.begin(), bullets.end(),
        [](const Bullet& b) { return !b.active; }), bullets.end());

    // Ensure health doesn't go below 0 or above maxHealth
    health = CLAMP(health, 0.0f, maxHealth);
}


void Player::draw() const {
    // Player visual feedback: Flashing when invulnerable OR hit
    bool isFlashing = (invulnerabilityTimer > 0 && (int)(GetTime() * 10) % 2 == 0) ||
                      (damageTakenFlashTimer > 0 && (int)(GetTime() * 20) % 2 == 0); // Faster flash for hit

    if (!isFlashing) { // Only draw stickman if not in a "blink" phase
        drawStickman(); 
    }
    
    weapon.draw(pos, facing); // Draw the weapon held by the player
    drawMuzzleFlash();       // Draw muzzle flash if active

    // Draw health bar above player (can be hidden if HUD is primary)
    drawHealthBar();

    // Draw bullets
    for (const auto& bullet : bullets) {
        if (bullet.active) bullet.draw();
    }
}


void Player::shoot(float currentTime) {
    if (weapon.canFire(currentTime)) {
        weapon.playFireSound();
        // Bullet originates slightly ahead of the player in the facing direction
        Vector2 bulletOrigin = Vector2Add(pos, Vector2Scale(Vector2Normalize(facing), size * 0.8f));
        Vector2 bulletVel = Vector2Normalize(facing);
        bulletVel.x *= weapon.bulletSpeed;
        bulletVel.y *= weapon.bulletSpeed;

        bullets.emplace_back(bulletOrigin, bulletVel, weapon.damage);
        muzzleFlashTimer = MUZZLE_FLASH_DURATION; // Activate muzzle flash
    }
}

void Player::takeDamage(float amount) {
    // Only take damage if not invulnerable and still alive
    if (invulnerabilityTimer <= 0 && health > 0) {
        health -= amount;
        invulnerabilityTimer = INVULNERABILITY_DURATION; // Start invulnerability
        damageTakenFlashTimer = DAMAGE_FLASH_DURATION; // Activate visual damage flash
        timeSinceLastDamage = 0.0f; // Reset regen timer
        // Future: Play damage sound effect, show blood overlay etc.
    }
}

void Player::dash(Vector2 direction) {
    if (!isDashing && Vector2Length(direction) > 0) { // Can only dash if not already dashing
        isDashing = true;
        dashTimer = DASH_DURATION;
        dashDirection = Vector2Normalize(direction); // Store normalized direction
        invulnerabilityTimer = DASH_DURATION; // Player is invulnerable during dash
    }
}

// --- Private Helper Functions (for internal logic) ---

void Player::updateInvulnerability(float deltaTime) {
    if (invulnerabilityTimer > 0) {
        invulnerabilityTimer -= deltaTime;
        if (invulnerabilityTimer < 0) invulnerabilityTimer = 0;
    }
}

void Player::updateMuzzleFlash(float deltaTime) {
    if (muzzleFlashTimer > 0) {
        muzzleFlashTimer -= deltaTime;
        if (muzzleFlashTimer < 0) muzzleFlashTimer = 0;
    }
}

void Player::updateDamageFlash(float deltaTime) {
    if (damageTakenFlashTimer > 0) {
        damageTakenFlashTimer -= deltaTime;
        if (damageTakenFlashTimer < 0) damageTakenFlashTimer = 0;
    }
}

void Player::updateDash(float deltaTime) {
    if (isDashing) {
        dashTimer -= deltaTime;
        if (dashTimer <= 0) {
            isDashing = false;
            dashTimer = 0;
        } else {
            // Move player rapidly in dash direction
            // Note: Wall collision for dash needs to be handled in main game loop or here carefully
            // For now, it's just movement. Main game loop should prevent collision.
            pos = Vector2Add(pos, Vector2Scale(dashDirection, 200.0f * DASH_SPEED_MULTIPLIER * deltaTime));
        }
    }
}

void Player::updateHealthRegen(float deltaTime) {
    if (health >= maxHealth) { // Don't regen if already full health
        timeSinceLastDamage = 0.0f; // Reset if full health
        regenTimer = 0.0f;
        return;
    }

    // Increase time since last damage
    timeSinceLastDamage += deltaTime;

    if (timeSinceLastDamage >= REGEN_COOLDOWN) {
        // Start regenerating health
        regenTimer += deltaTime;
        if (regenTimer >= REGEN_INTERVAL) {
            health += REGEN_AMOUNT;
            regenTimer -= REGEN_INTERVAL; // Subtract interval to keep track of remainder
            // Ensure health doesn't exceed max health
            health = CLAMP(health, 0.0f, maxHealth);
        }
    } else {
        // Not enough time passed since last damage, reset regen timer
        regenTimer = 0.0f;
    }
}


// --- Private Helper Functions (for Drawing) ---

void Player::drawStickman() const {
    Color skinColor = LIGHTGRAY;
    Color bodyColor = GRAY;
    Color armLegColor = GRAY;
    float limbThickness = 5;

    // Head
    float headRadius = size * 0.5f;
    Vector2 headCenter = { pos.x, pos.y - size * 0.8f };
    DrawCircleV(headCenter, headRadius, skinColor);

    // Body (Thicker line for a more defined torso)
    Vector2 bodyTop = { pos.x, pos.y - size * 0.4f };
    Vector2 bodyBottom = { pos.x, pos.y + size * 0.3f };
    DrawLineEx(bodyTop, bodyBottom, size * 0.8f, bodyColor);

    // Player color tint if just took damage (visual feedback)
    if (damageTakenFlashTimer > 0) {
        DrawCircleV(pos, size * 1.5f, ColorAlpha(RED, damageTakenFlashTimer / DAMAGE_FLASH_DURATION * 0.5f)); // Red tint
    }
    
    // Slight bobbing animation when not dashing
    float bobOffset = 0.0f;
    if (!isDashing) {
        bobOffset = sinf(GetTime() * 8.0f) * 2.0f; // Faster bobbing for 'walking'
    }

    // Calculate common joint points for arms and legs, applying bobOffset
    Vector2 shoulder = { pos.x, pos.y - size * 0.3f + bobOffset };
    Vector2 hip = { pos.x, pos.y + size * 0.2f + bobOffset };

    // Arms - Dynamic based on facing direction
    Vector2 rightHandOffset = Vector2Scale(Vector2Normalize(facing), size * 1.2f);
    Vector2 rightHand = Vector2Add(shoulder, rightHandOffset);
    DrawLineEx(shoulder, rightHand, limbThickness, armLegColor);

    Vector2 leftHand;
    Vector2 perpFacing = { -facing.y, facing.x };
    leftHand = Vector2Add(shoulder, Vector2Scale(perpFacing, size * 0.8f));
    DrawLineEx(shoulder, leftHand, limbThickness, armLegColor);

    // Legs - Simple fixed spread, creating a standing or slightly walking pose
    float legSpread = size * 0.4f;
    Vector2 leftFoot = { hip.x - legSpread, hip.y + size * 0.7f + bobOffset };
    Vector2 rightFoot = { hip.x + legSpread, hip.y + size * 0.7f + bobOffset };

    DrawLineEx(hip, leftFoot, limbThickness, armLegColor);
    DrawLineEx(hip, rightFoot, limbThickness, armLegColor);
}

void Player::drawHealthBar() const {
    float barWidth = size * 2.0f;
    float barHeight = 6;
    float hpPercent = CLAMP(health / maxHealth, 0.0f, 1.0f);
    Vector2 barPos = { pos.x - barWidth / 2, pos.y - size * 1.8f };

    DrawRectangle(barPos.x, barPos.y, barWidth, barHeight, DARKGRAY);
    DrawRectangle(barPos.x, barPos.y, barWidth * hpPercent, barHeight, GREEN);
    DrawRectangleLines(barPos.x, barPos.y, barWidth, barHeight, BLACK);
}

void Player::drawMuzzleFlash() const {
    if (muzzleFlashTimer > 0) {
        // Muzzle position slightly ahead of the player's weapon's end
        Vector2 muzzlePos = Vector2Add(pos, Vector2Scale(Vector2Normalize(facing), size * 2.0f));
        // Flash size and alpha (transparency) fade out over the duration
        float flashAlpha = muzzleFlashTimer / MUZZLE_FLASH_DURATION; // Fades from 1.0 to 0.0
        float flashRadius = 5.0f + (1.0f - flashAlpha) * 10.0f; // Starts small, grows, then fades

        Color flashColor = ColorAlpha(YELLOW, flashAlpha);

        // Draw a central glow and few lines
        DrawCircleV(muzzlePos, flashRadius, flashColor);
        DrawLineEx(Vector2Add(muzzlePos, Vector2Scale(Vector2Normalize(facing), -flashRadius * 0.5f)),
                   Vector2Add(muzzlePos, Vector2Scale(Vector2Normalize(facing), flashRadius * 1.5f)), 2, flashColor);
        // Perpendicular lines for star effect
        Vector2 perpFacing = { -facing.y, facing.x };
        DrawLineEx(Vector2Add(muzzlePos, Vector2Scale(perpFacing, -flashRadius * 0.5f)),
                   Vector2Add(muzzlePos, Vector2Scale(perpFacing, flashRadius * 0.5f)), 2, flashColor);
    }
}
