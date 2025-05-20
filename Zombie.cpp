#include "Zombie.h"
#include <algorithm>
#include "raymath.h"
// Helper function for wall collision
bool ZombieCollidesWithWall(Vector2 newPos, float size, const std::vector<Rectangle>& walls) {
    for (const auto& wall : walls) {
        if (CheckCollisionCircleRec(newPos, size, wall)) return true;
    }
    return false;
}


void Zombie::update(Vector2 playerPos, float deltaTime, const std::vector<Rectangle>& walls, float& playerHealth) {
    Vector2 dir = Vector2Normalize({ playerPos.x - pos.x, playerPos.y - pos.y });

    // Try to move directly toward the player
    Vector2 newPos = pos;
    newPos.x += dir.x * speed * deltaTime;
    newPos.y += dir.y * speed * deltaTime;

    // If direct movement is blocked, try small angled offsets (like steering)
    const float angleOffset = 0.3f; // ~17 degrees
    const int tryAngles = 5; // Try 5 offsets on each side

    bool moved = false;
    if (!ZombieCollidesWithWall(newPos, size, walls)) {
        pos = newPos;
        moved = true;
    } else {
        for (int i = 1; i <= tryAngles; ++i) {
            float angle = angleOffset * i;

            Vector2 leftDir = Vector2Rotate(dir, angle);
            Vector2 rightDir = Vector2Rotate(dir, -angle);

            Vector2 tryLeft = Vector2Add(pos, Vector2Scale(leftDir, speed * deltaTime));
            Vector2 tryRight = Vector2Add(pos, Vector2Scale(rightDir, speed * deltaTime));

            if (!ZombieCollidesWithWall(tryLeft, size, walls)) {
                pos = tryLeft;
                moved = true;
                break;
            } else if (!ZombieCollidesWithWall(tryRight, size, walls)) {
                pos = tryRight;
                moved = true;
                break;
            }
        }
    }

    // Damage player if touching
    float distToPlayer = Vector2Distance(pos, playerPos);
    if (distToPlayer < size + 20.0f) {
        playerHealth -= damage * deltaTime;
    }
}



void Zombie::draw() const {
    // Colors
    Color zombieSkin = bodyColor;
    Color zombieShadow = { 
        (unsigned char)(bodyColor.r * 0.3f),
        (unsigned char)(bodyColor.g * 0.5f),
        (unsigned char)(bodyColor.b * 0.3f),
        bodyColor.a
    };

    Color eyeWhite = WHITE;
    Color pupilColor = BLACK;
    Color teethColor = WHITE;
    Color mouthColor = { 60, 30, 30, 255 };

    // Head
    float headRadius = size * 0.6f;
    Vector2 headCenter = { pos.x, pos.y - size * 0.8f };

    // Head base
    DrawCircleV(headCenter, headRadius, zombieSkin);

    // Neck shadow (ellipse under head to separate body/head)
    DrawEllipse(headCenter.x, headCenter.y + headRadius * 0.5f, headRadius * 1.0f, headRadius * 0.3f, zombieShadow);

    // Eyes
    Vector2 leftEyePos = { headCenter.x + headRadius * 0.3f, headCenter.y - headRadius * 0.2f };
    Vector2 rightEyePos = { headCenter.x - headRadius * 0.3f, headCenter.y - headRadius * 0.2f };
    DrawCircleV(leftEyePos, headRadius * 0.25f, eyeWhite);
    DrawCircleV(rightEyePos, headRadius * 0.25f, eyeWhite);
    DrawCircleV(leftEyePos, headRadius * 0.1f, pupilColor);
    DrawCircleV(rightEyePos, headRadius * 0.1f, pupilColor);

    // Nose (triangle)
    Vector2 noseTip = { headCenter.x, headCenter.y };
    Vector2 noseLeft = { headCenter.x - headRadius * 0.1f, headCenter.y + headRadius * 0.15f };
    Vector2 noseRight = { headCenter.x + headRadius * 0.1f, headCenter.y + headRadius * 0.15f };
    DrawTriangle(noseTip, noseLeft, noseRight, zombieShadow);

    // Mouth (ellipse with teeth)
    Vector2 mouthCenter = { headCenter.x, headCenter.y + headRadius * 0.4f };
    float mouthWidth = headRadius * 0.6f;
    float mouthHeight = headRadius * 0.2f;

    DrawEllipse(mouthCenter.x, mouthCenter.y, mouthWidth, mouthHeight, mouthColor);

    // Teeth (small white rectangles inside mouth)
    int teethCount = 5;
    float teethWidth = mouthWidth / (teethCount * 1.5f);
    float teethHeight = mouthHeight * 0.7f;
    for (int i = 0; i < teethCount; i++) {
        float tx = mouthCenter.x - mouthWidth / 2 + i * teethWidth * 1.5f + teethWidth * 0.25f;
        float ty = mouthCenter.y - teethHeight / 2;
        Rectangle tooth = { tx, ty, teethWidth, teethHeight };
        DrawRectangleRec(tooth, teethColor);
    }

    // Head scar/stitches - short lines
    for (int i = 0; i < 3; i++) {
        float scarX = headCenter.x + headRadius * 0.5f;
        float scarY = headCenter.y - headRadius * 0.4f + i * headRadius * 0.15f;
        DrawLineEx({ scarX, scarY }, { scarX + headRadius * 0.2f, scarY + headRadius * 0.05f }, 2, DARKGRAY);
    }

    // Body (smaller oval, moved a bit down)
    Vector2 bodyPos = { pos.x, pos.y + size * 0.15f }; // move body down by 0.15*size
    Vector2 bodySize = { size * 0.6f, size * 0.7f };   // slimmer and shorter body
    DrawEllipse(bodyPos.x + 5, bodyPos.y + 5, bodySize.x, bodySize.y, zombieShadow); // Shadow
    DrawEllipse(bodyPos.x, bodyPos.y, bodySize.x, bodySize.y, zombieSkin);            // Body

    // Arms - smaller rectangles rotated slightly, moved outward
    float armWidth = size * 0.18f;
    float armHeight = size * 0.6f;
    Vector2 leftArmPos = { pos.x - bodySize.x * 0.7f - armWidth * 0.3f, pos.y + size * 0.1f };
    Vector2 rightArmPos = { pos.x + bodySize.x * 0.7f - armWidth * 0.7f, pos.y + size * 0.1f };

    DrawRectanglePro({ leftArmPos.x, leftArmPos.y, armWidth, armHeight }, { armWidth / 2, armHeight / 2 }, -15, zombieSkin);
    DrawRectanglePro({ rightArmPos.x, rightArmPos.y, armWidth, armHeight }, { armWidth / 2, armHeight / 2 }, 15, zombieSkin);

    // Legs - smaller rectangles, moved slightly outward
    float legWidth = size * 0.25f;
    float legHeight = size * 0.75f;
    Vector2 leftLegPos = { pos.x - legWidth * 0.8f, pos.y + bodySize.y * 0.8f };
    Vector2 rightLegPos = { pos.x + legWidth * 0.3f, pos.y + bodySize.y * 0.8f };

    DrawRectangle(leftLegPos.x, leftLegPos.y, legWidth, legHeight, zombieSkin);
    DrawRectangle(rightLegPos.x, rightLegPos.y, legWidth, legHeight, zombieSkin);

    // Health bar above head
    float barWidth = size * 1.5f;
    float barHeight = 6;
    float healthPercent = Clamp(health / 100.0f, 0.0f, 1.0f);

    Rectangle bgBar = { pos.x - barWidth / 2, headCenter.y - headRadius - 15, barWidth, barHeight };
    DrawRectangleRec(bgBar, RED);

    Rectangle fgBar = { bgBar.x, bgBar.y, barWidth * healthPercent, barHeight };
    DrawRectangleRec(fgBar, GREEN);
}


void Zombie::takeDamage(int dmg) {
    health -= dmg;
}
