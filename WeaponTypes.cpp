#include "WeaponTypes.h"

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
    DrawRectangle(x, y, 60, 20, DARKGRAY);     // body
    DrawRectangle(x + 45, y - 10, 15, 10, GRAY); // barrel
    DrawRectangle(x + 10, y + 20, 10, 20, BLACK); // grip
}

void DrawShotgunIcon(int x, int y) {
    DrawRectangle(x, y, 90, 15, BROWN);           // long barrel
    DrawRectangle(x, y + 10, 20, 20, DARKBROWN);  // stock
    DrawRectangle(x + 70, y - 5, 20, 5, GRAY);    // muzzle
}

void DrawRifleIcon(int x, int y) {
    DrawRectangle(x, y, 100, 10, DARKGREEN);     // barrel
    DrawRectangle(x + 70, y - 10, 10, 30, GREEN); // scope
    DrawRectangle(x, y + 10, 20, 20, BLACK);      // grip
}
