#include "Weapon.h"
#include <vector>
#include "raylib.h"


Weapon::Weapon(float fireRate, float bulletSpeed, int damage, WeaponType type)
    : fireRate(fireRate), bulletSpeed(bulletSpeed), damage(damage), type(type), lastFireTime(0.0f) {}

bool Weapon::canFire(float currentTime) {
    if (currentTime - lastFireTime >= 1.0f / fireRate) {
        lastFireTime = currentTime;
        return true;
    }
    return false;
}

void Weapon::draw(Vector2 playerPos, Vector2 facing) const {
    Vector2 normFacing = Vector2Normalize(facing);
    Vector2 right = { -normFacing.y, normFacing.x }; // perpendicular vector for thickness
    Vector2 weaponOffset = Vector2Scale(normFacing, 15); // bring it closer to hand
    Vector2 weaponPos = Vector2Add(playerPos, weaponOffset);
    float angle = atan2f(normFacing.y, normFacing.x) * RAD2DEG;

    switch (type) {
        case WeaponType::Pistol: {
            // Pistol handle + barrel
            DrawRectanglePro({ weaponPos.x, weaponPos.y, 25, 5 }, { 0, 2.5f }, angle, DARKGRAY);
            DrawRectanglePro({ weaponPos.x + 18, weaponPos.y - 5, 7, 5 }, { 0, 2.5f }, angle, GRAY); // muzzle
            DrawRectanglePro({ weaponPos.x - 4, weaponPos.y + 5, 6, 10 }, { 0, 0 }, angle, BLACK);   // grip
            break;
        }
        case WeaponType::Shotgun: {
            // Shotgun long barrel, stock, muzzle
            DrawRectanglePro({ weaponPos.x, weaponPos.y, 40, 6 }, { 0, 3 }, angle, BROWN);
            DrawRectanglePro({ weaponPos.x + 38, weaponPos.y - 3, 5, 3 }, { 0, 1.5f }, angle, GRAY); // muzzle
            DrawRectanglePro({ weaponPos.x - 10, weaponPos.y + 5, 10, 12 }, { 0, 0 }, angle, DARKBROWN); // grip/stock
            break;
        }
        case WeaponType::Rifle: {
            // Rifle body + scope + grip
            DrawRectanglePro({ weaponPos.x, weaponPos.y, 50, 4 }, { 0, 2 }, angle, DARKGREEN); // barrel
            DrawRectanglePro({ weaponPos.x + 25, weaponPos.y - 5, 10, 4 }, { 0, 2 }, angle, GREEN); // scope
            DrawRectanglePro({ weaponPos.x - 5, weaponPos.y + 4, 6, 10 }, { 0, 0 }, angle, BLACK); // grip
            break;
        }
    }
}
