#include "WeaponTypes.h"
#include "raymath.h" // Needed for Vector2 utilities in icon drawing

// WeaponTypes.cpp
Weapon CreatePistol() {
    return Weapon(3.0f, 600.0f, 20, WeaponType::Pistol);
}

Weapon CreateShotgun() {
    return Weapon(1.0f, 400.0f, 50, WeaponType::Shotgun);
}

Weapon CreateRifle() {
    return Weapon(5.0f, 800.0f, 15, WeaponType::Rifle);
}

void DrawPistolIcon(int x, int y) {
    Vector2 basePos = {(float)x, (float)y};
    float scale = 0.8f; // Adjust scale for icons if needed

    // Main body
    DrawRectangle(basePos.x, basePos.y + 5 * scale, 35 * scale, 10 * scale, GRAY);
    // Barrel
    DrawRectangle(basePos.x + 35 * scale, basePos.y + 7 * scale, 10 * scale, 6 * scale, DARKGRAY);
    // Grip (angled)
    // DrawRectanglePro signature: Rectangle rec, Vector2 origin, float rotation, Color color
    // Correctly define the Rectangle first: {x, y, width, height}
    DrawRectanglePro({basePos.x + 5 * scale, basePos.y + 15 * scale, 10 * scale, 20 * scale}, {0,0}, 15, BLACK);
    // Hammer
    DrawRectangle(basePos.x + 2 * scale, basePos.y + 3 * scale, 4 * scale, 5 * scale, LIGHTGRAY);
}

void DrawShotgunIcon(int x, int y) {
    Vector2 basePos = {(float)x, (float)y};
    float scale = 0.8f;

    // Receiver
    DrawRectangle(basePos.x, basePos.y + 5 * scale, 50 * scale, 10 * scale, BROWN);
    // Barrel
    DrawRectangle(basePos.x + 50 * scale, basePos.y + 7 * scale, 30 * scale, 6 * scale, DARKBROWN);
    // Pump action foregrip
    DrawRectangle(basePos.x + 30 * scale, basePos.y + 7 * scale, 15 * scale, 8 * scale, BROWN);
    // Stock base
    DrawRectanglePro({basePos.x + 10 * scale, basePos.y + 10 * scale, 25 * scale, 15 * scale}, {0,0}, -5, DARKBROWN);
    // Muzzle break
    DrawRectangle(basePos.x + 78 * scale, basePos.y + 6 * scale, 7 * scale, 8 * scale, GRAY);
}

void DrawRifleIcon(int x, int y) {
    Vector2 basePos = {(float)x, (float)y};
    float scale = 0.8f;

    // Receiver / Main Body
    DrawRectangle(basePos.x, basePos.y + 8 * scale, 60 * scale, 8 * scale, DARKGREEN);
    // Barrel
    DrawRectangle(basePos.x + 60 * scale, basePos.y + 9 * scale, 35 * scale, 5 * scale, BLACK);
    // Magazine
    DrawRectangle(basePos.x + 20 * scale, basePos.y + 16 * scale, 8 * scale, 20 * scale, DARKGRAY);
    // Scope
    DrawRectangle(basePos.x + 30 * scale, basePos.y, 25 * scale, 6 * scale, GRAY); // Scope body
    DrawCircle(basePos.x + 50 * scale, basePos.y + 3 * scale, 4 * scale, BLACK); // Front lens
    // Stock
    DrawRectanglePro({basePos.x - 5 * scale, basePos.y + 10 * scale, 35 * scale, 10 * scale}, {0,0}, 3, DARKGREEN);
    // Pistol Grip
    DrawRectanglePro({basePos.x + 5 * scale, basePos.y + 15 * scale, 10 * scale, 20 * scale}, {0,0}, 20, BLACK);
}