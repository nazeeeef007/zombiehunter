// Weapon.h
#pragma once
#include "raylib.h"
#include "raymath.h"

enum class WeaponType {
    Pistol,
    Shotgun,
    Rifle
};

class Weapon {
public:
    float fireRate;
    float bulletSpeed;
    int damage;
    WeaponType type;

    Weapon(float fireRate, float bulletSpeed, int damage, WeaponType type);

    bool canFire(float currentTime);
    void draw(Vector2 playerPos, Vector2 facing) const;

private:
    float lastFireTime;
};
