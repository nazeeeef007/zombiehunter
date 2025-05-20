#pragma once
#include "Weapon.h"
#include "raylib.h" // For drawing

// Factory functions
Weapon CreatePistol();
Weapon CreateShotgun();
Weapon CreateRifle();

// Drawing functions for weapon icons
void DrawPistolIcon(int x, int y);
void DrawShotgunIcon(int x, int y);
void DrawRifleIcon(int x, int y);
