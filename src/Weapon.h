#ifndef WEAPON_H
#define WEAPON_H

#include "raylib.h" // Essential for 'Sound' type
#include "raymath.h" // Essential for Vector2

// Enum definition: Declares the types of weapons
enum class WeaponType {
    Pistol,
    Shotgun,
    Rifle
};

// Weapon class declaration: Declares the structure and methods of a Weapon object
class Weapon {
public:
    // Constructor
    Weapon(float fireRate, float bulletSpeed, int damage, WeaponType type);
    
    // Rule of Five (or Zero): Custom destructor means we need to manage copy/move operations.
    // Deleting copy constructor and copy assignment operator to prevent shallow copies of 'fireSound'.
    Weapon(const Weapon&) = delete;            // No copying allowed
    Weapon& operator=(const Weapon&) = delete; // No copy assignment allowed

    // Move constructor: Transfers ownership of 'fireSound' from one Weapon to another.
    Weapon(Weapon&& other) noexcept;
    // Move assignment operator: Transfers ownership of 'fireSound' from one Weapon to another.
    Weapon& operator=(Weapon&& other) noexcept;

    // Destructor (crucial for freeing loaded sounds when the LAST owner goes out of scope)
    ~Weapon(); 

    // Method declarations
    bool canFire(float currentTime);
    void draw(Vector2 playerPos, Vector2 facing) const;
    void playFireSound() const; 

    // Member variables
    WeaponType type;
    float fireRate;
    float lastFireTime;
    float bulletSpeed;
    int damage;
    Sound fireSound; // Declares the Sound object for firing audio
};

#endif // WEAPON_H
