#include "raylib.h"      // <-- Always include raylib.h for its functions
#include "Weapon.h"      // For Weapon class and WeaponType enum declarations
#include "raymath.h"     // For Vector2 utilities
#include <utility>       // For std::move

// --- IMPORTANT: Icon drawing function DEFINITIONS DO NOT BELONG IN THIS FILE. ---
// --- They MUST only be defined in WeaponTypes.cpp.                             ---

// Weapon constructor: Initializes weapon properties and loads its specific firing sound
Weapon::Weapon(float fireRate, float bulletSpeed, int damage, WeaponType type)
    : fireRate(fireRate), bulletSpeed(bulletSpeed), damage(damage), type(type), lastFireTime(0.0f) {
    
    fireSound = { 0 }; // Initialize fireSound to an empty/null sound initially

    // Load the appropriate sound file based on the weapon type
    switch (type) {
        case WeaponType::Pistol:
            // IMPORTANT: Replace with your actual relative path to the audio file
            fireSound = LoadSound("./assets/audio/pistol_fire.wav"); 
            SetSoundVolume(fireSound, 0.5f); // Set desired volume
            break;
        case WeaponType::Shotgun:
            // IMPORTANT: Replace with your actual relative path to the audio file
            fireSound = LoadSound("./assets/audio/shotgun_fire.wav");
            SetSoundVolume(fireSound, 0.7f); 
            break;
        case WeaponType::Rifle:
            // IMPORTANT: Replace with your actual relative path to the audio file
            fireSound = LoadSound("./assets/audio/rifle_fire.wav");
            SetSoundVolume(fireSound, 0.6f); 
            break;
        default:
            TraceLog(LOG_WARNING, "WEAPON: Unknown weapon type created, no specific fire sound loaded.");
            break;
    }
}

// Move constructor: "Steals" the resources from another Weapon object
Weapon::Weapon(Weapon&& other) noexcept
    : type(other.type), fireRate(other.fireRate), lastFireTime(other.lastFireTime), 
      bulletSpeed(other.bulletSpeed), damage(other.damage), fireSound(other.fireSound) {
    
    // After moving, set the original's sound to {0} so its destructor doesn't unload it
    other.fireSound = { 0 }; 
}

// Move assignment operator: Handles moving resources when assigning one Weapon to another
Weapon& Weapon::operator=(Weapon&& other) noexcept {
    if (this != &other) { // Prevent self-assignment
        // First, release our own current resources if any
        if (fireSound.frameCount > 0) {
            UnloadSound(fireSound);
        }

        // Steal resources from 'other'
        type = other.type;
        fireRate = other.fireRate;
        lastFireTime = other.lastFireTime;
        bulletSpeed = other.bulletSpeed;
        damage = other.damage;
        fireSound = other.fireSound;

        // Clear 'other' so its destructor doesn't free the moved resource
        other.fireSound = { 0 };
    }
    return *this;
}

// Weapon destructor: Automatically unloads the sound resource when a Weapon object is destroyed
Weapon::~Weapon() {
    // Only unload if the sound resource is valid (i.e., hasn't been moved)
    if (fireSound.frameCount > 0) {
        UnloadSound(fireSound);
    }
}

// Method to check if the weapon can fire based on its fire rate
bool Weapon::canFire(float currentTime) {
    if (currentTime - lastFireTime >= 1.0f / fireRate) {
        lastFireTime = currentTime;
        return true;
    }
    return false;
}

// Method to play the weapon's firing sound
void Weapon::playFireSound() const {
    if (fireSound.frameCount > 0) { // Check if the sound resource is valid before playing
        PlaySound(fireSound);
    } else {
        TraceLog(LOG_WARNING, "WEAPON: Attempted to play an unloaded or invalid sound for weapon type %d", (int)type);
    }
}

// Method to draw the weapon on screen, considering player position and facing direction
void Weapon::draw(Vector2 playerPos, Vector2 facing) const {
    Vector2 normFacing = Vector2Normalize(facing);
    float angle = atan2f(normFacing.y, normFacing.x) * RAD2DEG;

    // Base offset from player's hand/center, slightly adjusted to make it look held
    Vector2 weaponOffset = Vector2Scale(normFacing, 10);
    Vector2 weaponPivot = Vector2Add(playerPos, weaponOffset); // This is roughly where player holds it

    switch (type) {
        case WeaponType::Pistol: {
            float width, height;
            Vector2 rectPos; // Top-left corner of the rectangle

            // Main body/slide
            width = 30; height = 8;
            rectPos = Vector2Subtract(weaponPivot, Vector2Scale(normFacing, 0)); // Align with pivot
            DrawRectanglePro({rectPos.x, rectPos.y - height/2, width, height}, {0, height/2}, angle, GRAY);

            // Barrel
            width = 10; height = 6;
            rectPos = Vector2Add(weaponPivot, Vector2Scale(normFacing, 30));
            DrawRectanglePro({rectPos.x, rectPos.y - height/2, width, height}, {0, height/2}, angle, DARKGRAY);

            // Grip (angled for ergonomic feel)
            width = 10; height = 20;
            rectPos = Vector2Add(weaponPivot, Vector2Scale(normFacing, -10));
            DrawRectanglePro({rectPos.x - width/2, rectPos.y - height/2, width, height}, {width/2, height/2}, angle + 15, BLACK);

            // Trigger Guard
            width = 5; height = 10;
            Vector2 triggerGuardPos = Vector2Add(weaponPivot, Vector2Scale(normFacing, 8)); // Pos relative to weaponPivot
            DrawRectanglePro({triggerGuardPos.x - width/2, triggerGuardPos.y - height/2, width, height}, {width/2, height/2}, angle, DARKGRAY);

            // Hammer (small detail at the back)
            width = 3; height = 5;
            Vector2 hammerPos = Vector2Add(weaponPivot, Vector2Scale(normFacing, -5));
            DrawRectanglePro({hammerPos.x - width/2, hammerPos.y - height/2, width, height}, {width/2, height/2}, angle, LIGHTGRAY);
            break;
        }
        case WeaponType::Shotgun: {
            float width, height;
            Vector2 rectPos;

            // Receiver / Pump action body
            width = 45; height = 10;
            rectPos = Vector2Subtract(weaponPivot, Vector2Scale(normFacing, 0));
            DrawRectanglePro({rectPos.x, rectPos.y - height/2, width, height}, {0, height/2}, angle, BROWN);

            // Barrel (long and thin)
            width = 25; height = 6;
            rectPos = Vector2Add(weaponPivot, Vector2Scale(normFacing, 45));
            DrawRectanglePro({rectPos.x, rectPos.y - height/2, width, height}, {0, height/2}, angle, DARKBROWN);

            // Pump action foregrip
            width = 15; height = 8;
            rectPos = Vector2Add(weaponPivot, Vector2Scale(normFacing, 25));
            DrawRectanglePro({rectPos.x, rectPos.y - height/2, width, height}, {0, height/2}, angle, BROWN);

            // Stock (more defined shape)
            width = 20; height = 15;
            Vector2 stockBase = Vector2Add(weaponPivot, Vector2Scale(normFacing, -30));
            DrawRectanglePro({stockBase.x - width/2, stockBase.y - height/2, width, height}, {width/2, height/2}, angle - 5, DARKBROWN);

            // Connecting part of stock (simpler rectangle aligned with receiver)
            width = 20; height = 8;
            rectPos = Vector2Add(weaponPivot, Vector2Scale(normFacing, -10)); // Position it between stockBase and receiver
            DrawRectanglePro({rectPos.x - width/2, rectPos.y - height/2, width, height}, {width/2, height/2}, angle, BROWN);


            // Muzzle break / Choke (slightly wider end)
            width = 5; height = 8;
            Vector2 muzzlePos = Vector2Add(weaponPivot, Vector2Scale(normFacing, 65));
            DrawRectanglePro({muzzlePos.x - width/2, muzzlePos.y - height/2, width, height}, {width/2, height/2}, angle, GRAY);
            break;
        }
        case WeaponType::Rifle: {
            float width, height;
            Vector2 rectPos;

            // Receiver / Main Body
            width = 50; height = 8;
            rectPos = Vector2Subtract(weaponPivot, Vector2Scale(normFacing, 0));
            DrawRectanglePro({rectPos.x, rectPos.y - height/2, width, height}, {0, height/2}, angle, DARKGREEN);

            // Barrel (long and slender)
            width = 40; height = 5;
            rectPos = Vector2Add(weaponPivot, Vector2Scale(normFacing, 50));
            DrawRectanglePro({rectPos.x, rectPos.y - height/2, width, height}, {0, height/2}, angle, BLACK);

            // Magazine (protruding downwards)
            width = 8; height = 25;
            Vector2 magOffset = Vector2Add(weaponPivot, Vector2Scale(normFacing, 10));
            DrawRectanglePro({magOffset.x - width/2, magOffset.y - height/2, width, height}, {width/2, height/2}, angle + 90, DARKGRAY);

            // Scope
            width = 25; height = 6;
            Vector2 scopeBase = Vector2Add(weaponPivot, Vector2Scale(normFacing, 15));
            DrawRectanglePro({scopeBase.x - width/2, scopeBase.y - height/2, width, height}, {width/2, height/2}, angle, GRAY); // Scope body
            DrawCircleV(Vector2Add(scopeBase, Vector2Scale(normFacing, 20)), 4, BLACK); // Front lens
            DrawCircleV(Vector2Add(scopeBase, Vector2Scale(normFacing, -5)), 3, BLACK); // Back lens

            // Stock
            width = 30; height = 10;
            Vector2 stockPos = Vector2Add(weaponPivot, Vector2Scale(normFacing, -30));
            DrawRectanglePro({stockPos.x - width/2, stockPos.y - height/2, width, height}, {width/2, height/2}, angle + 3, DARKGREEN);

            // Pistol Grip (angled for comfort)
            width = 10; height = 20;
            Vector2 pistolGripPos = Vector2Add(weaponPivot, Vector2Scale(normFacing, -5));
            DrawRectanglePro({pistolGripPos.x - width/2, pistolGripPos.y - height/2, width, height}, {width/2, height/2}, angle + 20, BLACK);
            break;
        }
    }
}
