#include "Zombie.h"
#include <algorithm>
#include "raymath.h"

#ifndef CLAMP
#define CLAMP(value, min, max) ((value < min) ? min : (value > max) ? max : value)
#endif

// --- Static Constant Definitions ---
const float Zombie::ZOMBIE_ATTACK_COOLDOWN = 1.0f;
const float Zombie::ZOMBIE_HIT_FLASH_DURATION = 0.1f;
const float Zombie::ZOMBIE_DEATH_DURATION = 0.5f;       // Shorter for more impactful explosion
const float Zombie::ZOMBIE_ATTACK_RANGE_BUFFER = 5.0f;
const float Zombie::ZOMBIE_EXPLOSION_MAX_RADIUS = 40.0f; // Max size of explosion particles/effect


// Helper function for wall collision (now a private member of Zombie)
bool Zombie::checkWallCollision(Vector2 checkPos, float checkSize, const std::vector<Rectangle>& walls) const {
    for (const auto& wall : walls) {
        if (CheckCollisionCircleRec(checkPos, checkSize, wall)) {
            return true;
        }
    }
    return false;
}

// Constructor
Zombie::Zombie(Vector2 pos, float speed, int health, int damage, float size, Color color)
    : pos(pos), speed(speed), health(health), maxHealth(health), damage(damage), size(size),
      bodyColor(color), currentState(ZombieState::CHASING),
      attackCooldownTimer(0.0f), hitFlashTimer(0.0f), deathTimer(0.0f),
      explosionRadius(0.0f), explosionAlpha(0.0f) {}


void Zombie::update(Vector2 playerPos, float deltaTime, const std::vector<Rectangle>& walls, float& playerHealth) {
    if (currentState == ZombieState::DEAD) {
        return; // No updates for fully dead zombies
    }

    updateTimers(deltaTime);

    switch (currentState) {
        case ZombieState::CHASING:
            handleChasingState(playerPos, deltaTime, walls);
            if (Vector2Distance(pos, playerPos) < size + 20.0f - ZOMBIE_ATTACK_RANGE_BUFFER && attackCooldownTimer <= 0) {
                currentState = ZombieState::ATTACKING;
            }
            break;
        case ZombieState::ATTACKING:
            handleAttackingState(playerPos, deltaTime, playerHealth);
            if (Vector2Distance(pos, playerPos) >= size + 20.0f || attackCooldownTimer > 0) {
                 currentState = ZombieState::CHASING;
            }
            break;
        case ZombieState::DYING:
            handleDyingState(deltaTime); // This will update explosion and deathTimer
            if (deathTimer <= 0) {
                currentState = ZombieState::DEAD; // Transition to fully DEAD
            }
            break;
        case ZombieState::DEAD:
            break; // This state is just a marker for removal by the main game loop
    }

    // Health check and state transition to DYING
    if (health <= 0 && currentState != ZombieState::DYING && currentState != ZombieState::DEAD) {
        currentState = ZombieState::DYING;
        deathTimer = ZOMBIE_DEATH_DURATION; // Start death animation timer
        explosionRadius = 0.0f; // Start explosion from 0
        explosionAlpha = 1.0f; // Start fully opaque
        // Play death sound here
    }

    // Ensure health doesn't go below 0 (important for drawing health bar)
    health = CLAMP(health, 0, maxHealth);
}

void Zombie::draw() const {
    if (currentState == ZombieState::DEAD) {
        return; // Don't draw fully dead zombies
    }

    // --- DRAWING THE EXPLOSION EFFECT IF DYING ---
    if (currentState == ZombieState::DYING) {
        drawExplosionEffect();
        return; // Don't draw the zombie body if it's exploding
    }

    // --- DRAWING THE LIVE ZOMBIE BODY ---
    // Apply hit flash (temporary white tint)
    Color finalDrawColor = bodyColor;
    if (hitFlashTimer > 0) {
        float flashRatio = hitFlashTimer / ZOMBIE_HIT_FLASH_DURATION; // 1.0 -> 0.0
        finalDrawColor = ColorLerp(WHITE, bodyColor, flashRatio); // Lerp towards white
    }
    
    // Ensure live zombie is fully opaque
    finalDrawColor.a = 255; 

    // --- SCARIER COLOR PALETTE ---
    Color zombieSkin = finalDrawColor;
    Color zombieShadow = { 
        (unsigned char)(finalDrawColor.r * 0.4f), // Darker, more muted
        (unsigned char)(finalDrawColor.g * 0.4f),
        (unsigned char)(finalDrawColor.b * 0.4f),
        finalDrawColor.a // Inherit alpha
    };

    Color eyeSclera = CLITERAL(Color){ 200, 150, 0, 255 }; // Yellowish/diseased eye white
    Color pupilColor = CLITERAL(Color){ 150, 0, 0, 255 }; // Dark red pupils
    Color teethColor = CLITERAL(Color){ 180, 160, 100, 255 }; // Yellowish brown teeth
    Color mouthColor = CLITERAL(Color){ 40, 20, 20, 255 }; // Darker, almost black mouth cavity
    Color woundColor = CLITERAL(Color){ 120, 0, 0, 255 }; // Dark blood/decay

    float currentSize = size;
    
    // Exaggerated bobbing for shambling movement
    float bobOffset = sinf(GetTime() * 12.0f) * 3.0f; // Faster, larger bobbing


    // --- HEAD ---
    float headRadius = currentSize * 0.6f;
    Vector2 headCenter = { pos.x, pos.y - currentSize * 0.8f + bobOffset };

    DrawCircleV(headCenter, headRadius, zombieSkin); // Head base
    DrawEllipse(headCenter.x, headCenter.y + headRadius * 0.5f, headRadius * 1.0f, headRadius * 0.3f, zombieShadow); // Neck shadow

    // Sunken eye sockets/cheeks (darker ellipses)
    DrawEllipse(headCenter.x + headRadius * 0.4f, headCenter.y - headRadius * 0.1f, headRadius * 0.3f, headRadius * 0.2f, zombieShadow);
    DrawEllipse(headCenter.x - headRadius * 0.4f, headCenter.y - headRadius * 0.1f, headRadius * 0.3f, headRadius * 0.2f, zombieShadow);


    // Eyes
    Vector2 leftEyePos = { headCenter.x + headRadius * 0.3f, headCenter.y - headRadius * 0.2f };
    Vector2 rightEyePos = { headCenter.x - headRadius * 0.3f, headCenter.y - headRadius * 0.2f };
    DrawCircleV(leftEyePos, headRadius * 0.25f, eyeSclera); // Yellowish sclera
    DrawCircleV(rightEyePos, headRadius * 0.25f, eyeSclera);
    DrawCircleV(leftEyePos, headRadius * 0.12f, pupilColor); // Larger red pupils
    DrawCircleV(rightEyePos, headRadius * 0.12f, pupilColor);

    // Nose (triangle)
    Vector2 noseTip = { headCenter.x, headCenter.y };
    Vector2 noseLeft = { headCenter.x - headRadius * 0.1f, headCenter.y + headRadius * 0.15f };
    Vector2 noseRight = { headCenter.x + headRadius * 0.1f, headCenter.y + headRadius * 0.15f };
    DrawTriangle(noseTip, noseLeft, noseRight, zombieShadow);

    // Mouth (more open, ragged look)
    Vector2 mouthCenter = { headCenter.x, headCenter.y + headRadius * 0.45f }; // Lowered slightly
    float mouthWidth = headRadius * 0.7f; // Wider
    float mouthHeight = headRadius * 0.3f; // Taller
    DrawEllipse(mouthCenter.x, mouthCenter.y, mouthWidth, mouthHeight, mouthColor);

    // Teeth (more jagged/irregular)
    int teethCount = 6;
    float teethWidth = mouthWidth / (teethCount * 1.2f);
    float teethHeight = mouthHeight * 0.8f;
    for (int i = 0; i < teethCount; i++) {
        float tx = mouthCenter.x - mouthWidth / 2 + i * teethWidth * 1.3f + teethWidth * 0.1f;
        float ty = mouthCenter.y - teethHeight / 2 + (i % 2 == 0 ? 0 : teethHeight * 0.1f);
        Rectangle tooth = { tx, ty, teethWidth, teethHeight };
        DrawRectangleRec(tooth, teethColor);
    }
    for (int i = 0; i < teethCount - 2; i++) {
        float tx = mouthCenter.x - mouthWidth / 2 + i * teethWidth * 1.5f + teethWidth * 0.5f;
        float ty = mouthCenter.y + teethHeight * 0.2f;
        Rectangle tooth = { tx, ty, teethWidth * 0.8f, teethHeight * 0.7f };
        DrawRectangleRec(tooth, teethColor);
    }

    // Head scar/wounds
    DrawLineEx({ headCenter.x + headRadius * 0.3f, headCenter.y - headRadius * 0.6f },
               { headCenter.x + headRadius * 0.7f, headCenter.y - headRadius * 0.5f }, 2, woundColor);
    DrawLineEx({ headCenter.x - headRadius * 0.2f, headCenter.y + headRadius * 0.1f },
               { headCenter.x - headRadius * 0.5f, headCenter.y + headRadius * 0.2f }, 2, woundColor);


    // --- BODY ---
    Vector2 bodyPos = { pos.x, pos.y + currentSize * 0.15f + bobOffset };
    Vector2 bodyShapeSize = { currentSize * 0.6f, currentSize * 0.7f };
    DrawEllipse(bodyPos.x + 5, bodyPos.y + 5, bodyShapeSize.x, bodyShapeSize.y, zombieShadow); // Shadow
    DrawEllipse(bodyPos.x, bodyPos.y, bodyShapeSize.x, bodyShapeSize.y, zombieSkin);           // Body

    // A hint of exposed rib or wound on body
    DrawCircle(bodyPos.x + bodyShapeSize.x * 0.2f, bodyPos.y - bodyShapeSize.y * 0.1f, currentSize * 0.1f, woundColor);
    DrawRectangle(bodyPos.x - bodyShapeSize.x * 0.3f, bodyPos.y + bodyShapeSize.y * 0.2f, currentSize * 0.2f, 3, woundColor);


    // --- ARMS ---
    float armWidth = currentSize * 0.18f;
    float armHeight = currentSize * 0.6f;
    Vector2 leftArmPos = { pos.x - bodyShapeSize.x * 0.7f - armWidth * 0.3f, pos.y + currentSize * 0.1f + bobOffset };
    Vector2 rightArmPos = { pos.x + bodyShapeSize.x * 0.7f - armWidth * 0.7f, pos.y + currentSize * 0.1f + bobOffset };

    DrawRectanglePro({ leftArmPos.x, leftArmPos.y, armWidth, armHeight }, { armWidth / 2, armHeight / 2 }, -20, zombieSkin); // More slumped
    DrawRectanglePro({ rightArmPos.x, rightArmPos.y, armWidth, armHeight }, { armWidth / 2, armHeight / 2 }, 20, zombieSkin);


    // --- LEGS ---
    float legWidth = currentSize * 0.25f;
    float legHeight = currentSize * 0.75f;
    Vector2 leftLegPos = { pos.x - legWidth * 0.8f, pos.y + bodyShapeSize.y * 0.8f + bobOffset };
    Vector2 rightLegPos = { pos.x + legWidth * 0.3f, pos.y + bodyShapeSize.y * 0.8f + bobOffset };

    DrawRectangle(leftLegPos.x, leftLegPos.y, legWidth, legHeight, zombieSkin);
    DrawRectangle(rightLegPos.x, rightLegPos.y, legWidth, legHeight, zombieSkin);

    // Health bar (only if not dying/dead)
    drawHealthBar(); // Health bar uses the un-faded health.
}


void Zombie::takeDamage(int dmg) {
    if (currentState == ZombieState::DYING || currentState == ZombieState::DEAD) {
        return;
    }
    health -= dmg;
    
    // Immediately stop hit flash if health drops to zero or below
    if (health <= 0) {
        hitFlashTimer = 0;
    } else {
        hitFlashTimer = ZOMBIE_HIT_FLASH_DURATION;
    }
}

// --- Private Helper Functions for Internal Logic ---

void Zombie::handleChasingState(Vector2 playerPos, float deltaTime, const std::vector<Rectangle>& walls) {
    Vector2 dir = Vector2Normalize(Vector2Subtract(playerPos, pos));
    float currentSpeed = speed;

    // Calculate desired movement for this frame
    Vector2 desiredMove = Vector2Scale(dir, currentSpeed * deltaTime);
    
    // Apply desired movement
    pos = Vector2Add(pos, desiredMove);

    // --- Robust Collision Resolution ---
    // Iterate multiple times to resolve cascades of collisions (e.g., in corners)
    // A small fixed number of iterations (e.g., 3-5) is usually sufficient
    for (int i = 0; i < 5; ++i) { // 5 iterations for robustness
        for (const auto& wall : walls) {
            resolveSingleWallCollision(pos, size, wall);
        }
    }
}

// NEW: Robust single circle-rectangle collision resolution
void Zombie::resolveSingleWallCollision(Vector2& circlePos, float circleRadius, const Rectangle& wall) const {
    if (!CheckCollisionCircleRec(circlePos, circleRadius, wall)) {
        return; // No collision, nothing to resolve
    }

    // Find the closest point on the rectangle to the circle's center
    float closestX = CLAMP(circlePos.x, wall.x, wall.x + wall.width);
    float closestY = CLAMP(circlePos.y, wall.y, wall.y + wall.height);

    Vector2 closestPoint = {closestX, closestY};
    
    Vector2 circleToClosest = Vector2Subtract(closestPoint, circlePos);
    float distance = Vector2Length(circleToClosest);

    // If distance is less than radius, there's an overlap
    if (distance < circleRadius) {
        // Calculate the overlap amount
        float overlap = circleRadius - distance;

        // If distance is effectively zero (circle center is inside the rect),
        // we need a different approach to get a valid normal.
        if (distance == 0.0f) {
            // This case means the circle's center is exactly on or inside the rectangle.
            // Determine the shortest axis to push out.
            float dx_min = closestX - circlePos.x; // signed distance from center to closest X point
            float dy_min = closestY - circlePos.y; // signed distance from center to closest Y point

            float overlap_x = circleRadius - fabsf(dx_min);
            float overlap_y = circleRadius - fabsf(dy_min);

            if (overlap_x < overlap_y) { // Push along X-axis
                if (dx_min < 0) { // Push left
                    circlePos.x = wall.x - circleRadius;
                } else { // Push right
                    circlePos.x = wall.x + wall.width + circleRadius;
                }
            } else { // Push along Y-axis
                if (dy_min < 0) { // Push up
                    circlePos.y = wall.y - circleRadius;
                } else { // Push down
                    circlePos.y = wall.y + wall.height + circleRadius;
                }
            }
        } else {
            // Normal case: push along the vector from the closest point to the circle's center
            Vector2 normal = Vector2Normalize(Vector2Negate(circleToClosest)); // Push away from the wall
            circlePos = Vector2Add(circlePos, Vector2Scale(normal, overlap));
        }
    }
}


void Zombie::handleAttackingState(Vector2 playerPos, float deltaTime, float& playerHealth) {
    float distToPlayer = Vector2Distance(pos, playerPos);
    if (distToPlayer < size + 20.0f - ZOMBIE_ATTACK_RANGE_BUFFER && attackCooldownTimer <= 0) {
        playerHealth -= damage;
        attackCooldownTimer = ZOMBIE_ATTACK_COOLDOWN;
    }
}

void Zombie::handleDyingState(float deltaTime) {
    deathTimer -= deltaTime;
    // Update explosion visual properties
    float t = CLAMP(1.0f - (deathTimer / ZOMBIE_DEATH_DURATION), 0.0f, 1.0f); // Progress from 0 to 1
    explosionRadius = ZOMBIE_EXPLOSION_MAX_RADIUS * t; // Radius grows
    explosionAlpha = CLAMP(1.0f - t, 0.0f, 1.0f); // Alpha fades out
}

void Zombie::updateTimers(float deltaTime) {
    if (attackCooldownTimer > 0) {
        attackCooldownTimer -= deltaTime;
        if (attackCooldownTimer < 0) attackCooldownTimer = 0;
    }
    if (hitFlashTimer > 0) {
        hitFlashTimer -= deltaTime;
        if (hitFlashTimer < 0) hitFlashTimer = 0;
    }
}

// --- Private Helper Functions for Drawing ---

void Zombie::drawHealthBar() const {
    // Only draw health bar if not dying and health > 0
    if (currentState == ZombieState::DYING || health <= 0) return;

    float barWidth = size * 1.5f;
    float barHeight = 6;
    float healthPercent = CLAMP(health / (float)maxHealth, 0.0f, 1.0f);

    Vector2 headCenter = { pos.x, pos.y - size * 0.8f }; // Re-calculate head center for positioning
    float headRadius = size * 0.6f;

    Rectangle bgBar = { pos.x - barWidth / 2, headCenter.y - headRadius - 15, barWidth, barHeight };
    DrawRectangleRec(bgBar, RED);

    Rectangle fgBar = { bgBar.x, bgBar.y, barWidth * healthPercent, barHeight };
    DrawRectangleRec(fgBar, GREEN);
}

void Zombie::drawExplosionEffect() const {
    // Draw multiple concentric, fading circles for the explosion
    int numCircles = 3;
    for (int i = 0; i < numCircles; ++i) {
        float currentExplosionRadius = explosionRadius * (1.0f - (float)i / numCircles);
        float currentExplosionAlpha = explosionAlpha * (1.0f - (float)i / numCircles);
        
        // Vary colors
        Color color1 = ColorAlpha(ORANGE, (unsigned char)(currentExplosionAlpha * 255));
        Color color2 = ColorAlpha(RED, (unsigned char)(currentExplosionAlpha * 0.7f * 255));
        Color color3 = ColorAlpha(DARKGRAY, (unsigned char)(currentExplosionAlpha * 0.4f * 255));

        DrawCircleV(pos, currentExplosionRadius, color1);
        DrawCircleLines(pos.x, pos.y, currentExplosionRadius * 0.8f, color2);
        DrawCircleLines(pos.x, pos.y, currentExplosionRadius * 0.4f, color3);
    }

    // Optional: Draw few small particles bursting outwards
    int numParticles = 8;
    for (int i = 0; i < numParticles; ++i) {
        float angle = (float)i * (360.0f / numParticles) * DEG2RAD;
        Vector2 particleDir = { cosf(angle), sinf(angle) };
        // Particles move faster as explosion expands
        Vector2 particlePos = Vector2Add(pos, Vector2Scale(particleDir, explosionRadius * 0.7f)); 
        DrawCircleV(particlePos, 2 + explosionRadius * 0.05f, ColorAlpha(YELLOW, (unsigned char)(explosionAlpha * 255)));
    }
}