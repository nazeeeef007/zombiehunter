#include "raylib.h"
#include "Player.h"
#include "Weapon.h"
#include "ZombieTypes.h" // Assumed to define FastZombie, TankZombie, etc.
#include "WeaponTypes.h" // Assumed to define CreatePistol, CreateShotgun, CreateRifle, WeaponType enum
#include <algorithm>
#include "raymath.h"
#include <vector>
#include <memory> // For std::unique_ptr, if you eventually use it for entities
#include <cstdlib> // For srand, rand
#include <ctime> // For time (to seed srand)
#include "CollisionUtils.h" // Assumed to have CollidesWithWallCircle
#include <string> // Added for std::string usage in DrawGameHUD
#include <utility> // For std::move

// --- Constants ---
const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 800; // Adjusted height for better 16:9 aspect or similar, feel free to change back

const int MAX_FLOORS = 3;
const int ZOMBIES_PER_FLOOR = 20;

// --- Helper Functions ---
// Clamps a float value between a minimum and maximum
float ClampFloat(float val, float minVal, float maxVal) {
    return (val < minVal) ? minVal : (val > maxVal) ? maxVal : val;
}

// Seeds the random number generator
void SeedRandom() {
    srand(static_cast<unsigned>(time(nullptr)));
}

// Creates a vector of random wall rectangles, ensuring no overlaps
std::vector<Rectangle> CreateWalls(int targetWallCount = 14) {
    std::vector<Rectangle> walls;
    int maxAttemptsPerWall = 100; // Max attempts to place each individual wall

    // Define a clearance area around the player's initial spawn point to ensure walls don't block it
    // Assuming player starts roughly at SCREEN_WIDTH/2, SCREEN_HEIGHT/2 with radius 20
    Rectangle playerSpawnClearance = {SCREEN_WIDTH / 2 - 40, SCREEN_HEIGHT / 2 - 40, 80, 80}; 

    for (int i = 0; i < targetWallCount; ++i) {
        int attempts = 0;
        bool placed = false;
        while (attempts < maxAttemptsPerWall && !placed) {
            float x = static_cast<float>(GetRandomValue(50, SCREEN_WIDTH - 150)); // Avoid edges
            float y = static_cast<float>(GetRandomValue(50, SCREEN_HEIGHT - 150)); // Avoid edges
            bool horizontal = GetRandomValue(0, 1);
            float width = horizontal ? static_cast<float>(GetRandomValue(80, 250)) : 20.0f;
            float height = horizontal ? 20.0f : static_cast<float>(GetRandomValue(80, 250));
            Rectangle newWall = { x, y, width, height };

            bool overlaps = false;
            // Check collision with existing walls
            for (const auto& existingWall : walls) {
                if (CheckCollisionRecs(newWall, existingWall)) {
                    overlaps = true;
                    break;
                }
            }

            // Ensure new walls don't overlap with the player's initial spawn area
            if (CheckCollisionRecs(newWall, playerSpawnClearance)) {
                overlaps = true;
            }

            if (!overlaps) {
                walls.push_back(newWall);
                placed = true;
            }
            attempts++;
        }
        // If 'placed' is false after maxAttemptsPerWall, it means we couldn't place this wall,
        // so we simply skip it and move to the next wall (if any remaining for targetWallCount).
    }
    return walls;
}

// --- Game States ---
enum GameState {
    SELECTING_WEAPON,
    PLAYING,
    GAME_OVER,
    GAME_WIN
};

// --- Custom Icon Drawing Functions (from your WeaponTypes.cpp) ---
// These are assumed to be defined in WeaponTypes.cpp and their declarations
// in WeaponTypes.h. Removed placeholder implementations from here to avoid
// multiple definition errors when linking with WeaponTypes.cpp.
void DrawPistolIcon(int x, int y);
void DrawShotgunIcon(int x, int y);
void DrawRifleIcon(int x, int y);


// --- Drawing Functions for UI ---

// Improved Weapon Selection Screen
void DrawWeaponSelectionScreen(float time, WeaponType& hoveredWeapon) { // hoveredWeapon is passed by reference to update it
    ClearBackground(Color{20, 20, 20, 255}); // Darker, more atmospheric background

    // Title
    DrawTextEx(GetFontDefault(), "CHOOSE YOUR WEAPON",
               (Vector2){(float)SCREEN_WIDTH / 2 - MeasureText("CHOOSE YOUR WEAPON", 50) / 2, 50},
               50, 2, GOLD); // Larger, more prominent title

    // Weapon Cards Layout
    float cardWidth = 300;
    float cardHeight = 400;
    float padding = 40;
    float startX = (SCREEN_WIDTH - (cardWidth * 3 + padding * 2)) / 2;
    float cardY = 150;

    Rectangle pistolRect = {startX, cardY, cardWidth, cardHeight};
    Rectangle shotgunRect = {startX + cardWidth + padding, cardY, cardWidth, cardHeight};
    Rectangle rifleRect = {startX + (cardWidth + padding) * 2, cardY, cardWidth, cardHeight};

    // Draw Card UI with highlight if hovered
    auto DrawCard = [&](Rectangle rect, Color baseColor, const char* name, const char* stats[], int statCount, void (*DrawIcon)(int, int), bool isHovered) {
        // Base background color, slightly transparent
        Color bgColor = Fade(baseColor, isHovered ? 0.9f : 0.6f);
        // Border color is brighter if hovered, otherwise a darker version of the base color
        Color borderColor = isHovered ? RAYWHITE : ColorAlpha(baseColor, 0.8f);
        float radius = 0.15f; // More rounded corners for a modern look

        // Draw a subtle shadow behind the card
        DrawRectangleRounded({rect.x + 8, rect.y + 8, rect.width, rect.height}, radius, 10, ColorAlpha(BLACK, 0.4f));
        // Draw the main card background
        DrawRectangleRounded(rect, radius, 10, bgColor);
        // Draw the card border
        DrawRectangleRoundedLines(rect, radius, 10, borderColor); 

        // Name of the weapon
        DrawText(name, rect.x + 20, rect.y + 20, 30, WHITE);

        // Icon (Adjust position for the icons which have their own internal sizing)
        // Icon is centered horizontally within the card
        DrawIcon(rect.x + (int)(rect.width / 2) - 30, rect.y + 80); 

        // Stats list
        for (int i = 0; i < statCount; ++i) {
            DrawText(stats[i], rect.x + 20, rect.y + 250 + i * 25, 20, LIGHTGRAY);
        }

        // Selection Hint, appears only when hovered
        if (isHovered) {
             DrawText("Click or Press Key", rect.x + (int)(rect.width/2) - MeasureText("Click or Press Key", 20)/2, rect.y + rect.height - 40, 20, YELLOW);
        }
    };

    // Define stats for each weapon type
    const char* pistolStats[] = {"FIRE RATE: 3.0/s", "BULLET SPEED: 600", "DAMAGE: 20"}; // From your WeaponTypes.cpp
    const char* shotgunStats[] = {"FIRE RATE: 1.0/s", "BULLET SPEED: 400", "DAMAGE: 50"};
    const char* rifleStats[] = {"FIRE RATE: 5.0/s", "BULLET SPEED: 800", "DAMAGE: 15"};

    // Determine which weapon card is currently hovered over by the mouse
    Vector2 mouse = GetMousePosition();
    WeaponType currentHoveredWeapon;
    if (CheckCollisionPointRec(mouse, pistolRect)) currentHoveredWeapon = WeaponType::Pistol;
    else if (CheckCollisionPointRec(mouse, shotgunRect)) currentHoveredWeapon = WeaponType::Shotgun;
    else if (CheckCollisionPointRec(mouse, rifleRect)) currentHoveredWeapon = WeaponType::Rifle;
    else currentHoveredWeapon = hoveredWeapon; // Keep the last hovered weapon if mouse moves off all cards
    hoveredWeapon = currentHoveredWeapon; // Update the reference passed to the function

    // Draw each weapon card, passing true for isHovered if it matches the current hoveredWeapon
    DrawCard(pistolRect, SKYBLUE, "1. PISTOL", pistolStats, 3, DrawPistolIcon, hoveredWeapon == WeaponType::Pistol);
    DrawCard(shotgunRect, RED, "2. SHOTGUN", shotgunStats, 3, DrawShotgunIcon, hoveredWeapon == WeaponType::Shotgun);
    DrawCard(rifleRect, GREEN, "3. RIFLE", rifleStats, 3, DrawRifleIcon, hoveredWeapon == WeaponType::Rifle);

    // Preview Stickman + Weapon at bottom to demonstrate the chosen weapon
    float animOffset = sinf(time * 4) * 5; // Smaller, smoother bounce animation
    Vector2 previewPos = {SCREEN_WIDTH / 2, cardY + cardHeight + 80 + animOffset};

    // Stickman body (more stylized)
    DrawCircleV(previewPos, 18, LIGHTGRAY); // head
    DrawRectangle(previewPos.x - 10, previewPos.y + 18, 20, 40, GRAY); // body
    DrawLineV({previewPos.x - 10, previewPos.y + 25}, {previewPos.x - 30, previewPos.y + 40}, WHITE); // left arm
    DrawLineV({previewPos.x + 10, previewPos.y + 25}, {previewPos.x + 30, previewPos.y + 40}, WHITE); // right arm
    DrawLineV({previewPos.x - 10, previewPos.y + 58}, {previewPos.x - 20, previewPos.y + 80}, DARKGRAY); // left leg
    DrawLineV({previewPos.x + 10, previewPos.y + 58}, {previewPos.x + 20, previewPos.y + 80}, DARKGRAY); // right leg

    // Weapon Preview (calling the same drawing functions but offset for preview)
    // Positioned near the stickman's hand
    Vector2 weaponPreviewPos = {previewPos.x - 30, previewPos.y + 20}; 
    switch (hoveredWeapon) {
        case WeaponType::Pistol:
            DrawPistolIcon(weaponPreviewPos.x, weaponPreviewPos.y);
            break;
        case WeaponType::Shotgun:
            DrawShotgunIcon(weaponPreviewPos.x, weaponPreviewPos.y);
            break;
        case WeaponType::Rifle:
            DrawRifleIcon(weaponPreviewPos.x, weaponPreviewPos.y);
            break;
        default: 
            DrawPistolIcon(weaponPreviewPos.x, weaponPreviewPos.y); // Fallback to pistol preview
            break;
    }
}


// Improved In-Game HUD
void DrawGameHUD(const Player& player, int currentFloor, int maxFloors, int zombiesKilled, int zombiesPerFloor) {
    // Top Bar Background: Semi-transparent dark bar across the top
    DrawRectangle(0, 0, SCREEN_WIDTH, 70, ColorAlpha(BLACK, 0.7f)); 

    // Game Title
    DrawTextEx(GetFontDefault(), "ZOMBIE SURVIVAL", (Vector2){20, 18}, 30, 2, LIME);

    // Floor Progress display
    DrawTextEx(GetFontDefault(), TextFormat("FLOOR: %d/%d", currentFloor, maxFloors),
               (Vector2){SCREEN_WIDTH / 2 - 150, 25}, 25, 2, GOLD);

    // Zombies Killed / Remaining (dynamic color based on progress)
    // Turns green when all zombies for the floor are killed
    Color zombieCountColor = (zombiesKilled >= zombiesPerFloor) ? GREEN : ORANGE;
    DrawTextEx(GetFontDefault(), TextFormat("ZOMBIES KILLED: %d/%d", zombiesKilled, zombiesPerFloor),
               (Vector2){SCREEN_WIDTH / 2 + 50, 25}, 25, 2, zombieCountColor);


    // Player Health Bar (Bottom Left)
    float healthBarWidth = 200;
    float healthBarHeight = 25;
    float healthX = 20;
    float healthY = SCREEN_HEIGHT - 40;

    // Background for health bar
    DrawRectangle(healthX, healthY, healthBarWidth, healthBarHeight, ColorAlpha(DARKGRAY, 0.8f));
    // Actual health fill, clamped to prevent drawing outside bounds
    float currentHealthWidth = ClampFloat(player.health / 100.0f, 0.0f, 1.0f) * healthBarWidth;
    // Health bar color changes based on remaining health
    Color healthColor = LIME;
    if (player.health < 60) healthColor = YELLOW;
    if (player.health < 30) healthColor = RED;
    DrawRectangle(healthX, healthY, currentHealthWidth, healthBarHeight, healthColor);
    // Health bar border
    DrawRectangleLinesEx({healthX, healthY, healthBarWidth, healthBarHeight}, 2, BLACK);
    // Health Text overlay
    DrawTextEx(GetFontDefault(), TextFormat("HP: %d", (int)player.health),
               (Vector2){healthX + healthBarWidth / 2 - MeasureText(TextFormat("HP: %d", (int)player.health), 20)/2, healthY + 3},
               20, 2, WHITE);

    // Current Weapon Display (Bottom Right)
    float weaponDisplayWidth = 180;
    float weaponDisplayHeight = 25;
    float weaponX = SCREEN_WIDTH - weaponDisplayWidth - 20;
    float weaponY = SCREEN_HEIGHT - 40;

    DrawRectangle(weaponX, weaponY, weaponDisplayWidth, weaponDisplayHeight, ColorAlpha(DARKGRAY, 0.8f));
    // Retrieve weapon name based on type enum
    std::string weaponName;
    switch(player.weapon.type) { 
        case WeaponType::Pistol: weaponName = "Pistol"; break;
        case WeaponType::Shotgun: weaponName = "Shotgun"; break;
        case WeaponType::Rifle: weaponName = "Rifle"; break;
        default: weaponName = "Unknown"; break;
    }
    DrawTextEx(GetFontDefault(), TextFormat("WEAPON: %s", weaponName.c_str()),
               (Vector2){weaponX + 10, weaponY + 3}, 20, 2, RAYWHITE);
    DrawRectangleLinesEx({weaponX, weaponY, weaponDisplayWidth, weaponDisplayHeight}, 2, BLACK);


    // Crosshair (more refined, separated lines)
    Vector2 mouse = GetMousePosition();
    float crosshairSize = 15.0f; // Length of each crosshair arm
    float gap = 3.0f; // Gap from the center
    Color crosshairColor = RED;

    // Horizontal lines
    DrawLineEx({mouse.x - crosshairSize, mouse.y}, {mouse.x - gap, mouse.y}, 2, crosshairColor);
    DrawLineEx({mouse.x + gap, mouse.y}, {mouse.x + crosshairSize, mouse.y}, 2, crosshairColor);
    // Vertical lines
    DrawLineEx({mouse.x, mouse.y - crosshairSize}, {mouse.x, mouse.y - gap}, 2, crosshairColor);
    DrawLineEx({mouse.x, mouse.y + gap}, {mouse.x, mouse.y + crosshairSize}, 2, crosshairColor);
}

// Draw Game Over Screen
void DrawGameOverScreen() {
    ClearBackground(Color{30, 0, 0, 255}); // Dark red background for game over

    // "YOU ARE DEAD!" text with a shadow effect
    DrawTextEx(GetFontDefault(), "YOU ARE DEAD!",
               (Vector2){(float)SCREEN_WIDTH / 2 - MeasureText("YOU ARE DEAD!", 80) / 2, SCREEN_HEIGHT / 2 - 80},
               80, 2, DARKGRAY); // Shadow text
    DrawTextEx(GetFontDefault(), "YOU ARE DEAD!",
               (Vector2){(float)SCREEN_WIDTH / 2 - MeasureText("YOU ARE DEAD!", 80) / 2 - 5, SCREEN_HEIGHT / 2 - 85},
               80, 2, RED); // Main text

    // "GAME OVER" with skull emojis for extra flair
    DrawTextEx(GetFontDefault(), "💀 GAME OVER 💀",
               (Vector2){(float)SCREEN_WIDTH / 2 - MeasureText("💀 GAME OVER 💀", 50) / 2, SCREEN_HEIGHT / 2 + 20},
               50, 2, WHITE);
    // Restart hint
    DrawTextEx(GetFontDefault(), "Press R to Restart",
               (Vector2){(float)SCREEN_WIDTH / 2 - MeasureText("Press R to Restart", 30) / 2, SCREEN_HEIGHT / 2 + 100},
               30, 2, LIGHTGRAY);
}

// Draw Game Win Screen
void DrawGameWinScreen() {
    ClearBackground(Color{0, 30, 0, 255}); // Dark green background for win

    // "CONGRATULATIONS!" text with a shadow effect
    DrawTextEx(GetFontDefault(), "CONGRATULATIONS!",
               (Vector2){(float)SCREEN_WIDTH / 2 - MeasureText("CONGRATULATIONS!", 80) / 2, SCREEN_HEIGHT / 2 - 80},
               80, 2, DARKGREEN); // Shadow
    DrawTextEx(GetFontDefault(), "CONGRATULATIONS!",
               (Vector2){(float)SCREEN_WIDTH / 2 - MeasureText("CONGRATULATIONS!", 80) / 2 - 5, SCREEN_HEIGHT / 2 - 85},
               80, 2, LIME); // Main

    // "ALL FLOORS CLEARED! YOU WIN!" with trophy emojis
    DrawTextEx(GetFontDefault(), "🏆 ALL FLOORS CLEARED! YOU WIN! 🏆",
               (Vector2){(float)SCREEN_WIDTH / 2 - MeasureText("🏆 ALL FLOORS CLEARED! YOU WIN! 🏆", 40) / 2, SCREEN_HEIGHT / 2 + 20},
               40, 2, GOLD);
    // Play Again hint
    DrawTextEx(GetFontDefault(), "Press R to Play Again",
               (Vector2){(float)SCREEN_WIDTH / 2 - MeasureText("Press R to Play Again", 30) / 2, SCREEN_HEIGHT / 2 + 100},
               30, 2, LIGHTGRAY);
}


// --- Main Game Loop ---
int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "🧟 Zombie Survival");
    
    // ADDED: Initialize the audio device right after window creation
    InitAudioDevice(); 

    SetTargetFPS(60);
    SeedRandom(); // Use the refined seeding function

    GameState gameState = SELECTING_WEAPON;

    // Game variables - FIX: Initialize selectedWeapon and player immediately
    // CORRECTED: Use std::move() when initializing player with selectedWeapon
    Weapon selectedWeapon = CreatePistol(); // Create the weapon (lvalue)
    Player player({ (float)SCREEN_WIDTH / 2, (float)SCREEN_HEIGHT / 2 }, { 1, 0 }, 20.0f, 100, std::move(selectedWeapon));
    
    std::vector<Rectangle> walls;
    std::vector<Zombie> zombies;
    float spawnTimer;
    float spawnInterval;
    int currentFloor;
    int zombiesKilled;

    // For weapon selection screen
    float uiTime = 0.0f; // Separate time for UI animations
    WeaponType hoveredWeapon = WeaponType::Pistol; // Default hovered weapon

    // --- Game Initialization function ---
    // Encapsulate game setup for restarts, making it easier to reset the game state
    auto InitializeGame = [&]() {
        // Player now re-initialized with the currently selected weapon
        // Note: When 'selectedWeapon' is re-assigned here, the old Weapon object's
        // destructor will be called (if defined), which should unload its sound.
        // The new Weapon object will then load its sound in its constructor.
        // CORRECTED: Use std::move() when re-initializing player with selectedWeapon
        player = Player({ (float)SCREEN_WIDTH / 2, (float)SCREEN_HEIGHT / 2 }, { 1, 0 }, 20.0f, 100, std::move(selectedWeapon));

        walls = CreateWalls(14); // Regenerate walls for each new floor/game
        zombies.clear(); // Clear any existing zombies
        spawnTimer = 0.0f;
        spawnInterval = 2.0f; // Initial spawn interval
        currentFloor = 1;
        zombiesKilled = 0;

        // Reset random seed for reproducible wall layouts or specific map seeds if desired
        // srand(static_cast<unsigned>(time(nullptr))); 
    };

    InitializeGame(); // Call once at the beginning to set up initial game state

    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();
        uiTime += deltaTime; // Update UI animation time

        BeginDrawing();

        switch (gameState) {
            case SELECTING_WEAPON:
                DrawWeaponSelectionScreen(uiTime, hoveredWeapon); // Pass uiTime for animation

                // Handle weapon selection inputs (keyboard and mouse click on hovered card)
                // If a key (1, 2, 3) is pressed OR the left mouse button is pressed while hovering over a card
                if (IsKeyPressed(KEY_ONE) || (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && hoveredWeapon == WeaponType::Pistol)) {
                    selectedWeapon = CreatePistol();
                    InitializeGame(); // Re-initialize game state, but retain the chosen weapon
                    gameState = PLAYING;
                }
                if (IsKeyPressed(KEY_TWO) || (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && hoveredWeapon == WeaponType::Shotgun)) {
                    selectedWeapon = CreateShotgun();
                    InitializeGame();
                    gameState = PLAYING;
                }
                if (IsKeyPressed(KEY_THREE) || (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && hoveredWeapon == WeaponType::Rifle)) {
                    selectedWeapon = CreateRifle();
                    InitializeGame();
                    gameState = PLAYING;
                }
                break;

            case PLAYING: {
                // Dungeon floor: dark concrete/stone background
                ClearBackground(Color{ 35, 30, 25, 255 }); 

                // Add subtle grid lines for stone tiles to give it a dungeon feel
                int gridSize = 100; // Size of each "tile"
                Color gridColor = ColorAlpha(Color{50, 45, 40, 255}, 0.5f); // Darker, semi-transparent lines

                for (int x = 0; x < SCREEN_WIDTH; x += gridSize) {
                    DrawLine(x, 0, x, SCREEN_HEIGHT, gridColor);
                }
                for (int y = 0; y < SCREEN_HEIGHT; y += gridSize) {
                    DrawLine(0, y, SCREEN_WIDTH, y, gridColor);
                }

                // Add a subtle vignette effect around the edges for atmosphere
                DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT / 4, ColorAlpha(BLACK, 0.4f)); // Top fade
                DrawRectangle(0, SCREEN_HEIGHT - SCREEN_HEIGHT / 4, SCREEN_WIDTH, SCREEN_HEIGHT / 4, ColorAlpha(BLACK, 0.4f)); // Bottom fade
                DrawRectangle(0, 0, SCREEN_WIDTH / 4, SCREEN_HEIGHT, ColorAlpha(BLACK, 0.4f)); // Left fade
                DrawRectangle(SCREEN_WIDTH - SCREEN_WIDTH / 4, 0, SCREEN_WIDTH / 4, SCREEN_HEIGHT, ColorAlpha(BLACK, 0.4f)); // Right fade


                // Input Handling for player movement
                Vector2 move = { 0, 0 };
                if (IsKeyDown(KEY_W)) move.y -= 1;
                if (IsKeyDown(KEY_S)) move.y += 1;
                if (IsKeyDown(KEY_D)) move.x += 1;
                if (IsKeyDown(KEY_A)) move.x -= 1;
                if (Vector2Length(move) > 0) move = Vector2Normalize(move); // Normalize diagonal movement

                float moveSpeed = 200.0f;
                Vector2 newPlayerPos = Vector2Add(player.pos, Vector2Scale(move, moveSpeed * deltaTime));

                // Check for wall collisions before updating player position
                if (!CollidesWithWallCircle(newPlayerPos, player.size, walls)) {
                    player.pos = newPlayerPos;
                }

                // Player facing direction towards mouse
                Vector2 mouse = GetMousePosition();
                player.facing = Vector2Normalize(Vector2Subtract(mouse, player.pos));

                // Shooting with left mouse button
                if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
                    player.shoot(GetTime());
                }

                // Zombie Spawning Logic
                spawnTimer += deltaTime;
                if (spawnTimer >= spawnInterval && (int)zombies.size() < ZOMBIES_PER_FLOOR) {
                    spawnTimer = 0.0f;
                    // Make zombies spawn randomly around the screen edges
                    Vector2 spawnPos;
                    int side = GetRandomValue(0, 3); // 0: top, 1: bottom, 2: left, 3: right
                    switch (side) {
                        case 0: spawnPos = { (float)GetRandomValue(0, SCREEN_WIDTH), -50.0f }; break; // Top edge, slightly off-screen
                        case 1: spawnPos = { (float)GetRandomValue(0, SCREEN_WIDTH), (float)SCREEN_HEIGHT + 50.0f }; break; // Bottom edge
                        case 2: spawnPos = { -50.0f, (float)GetRandomValue(0, SCREEN_HEIGHT) }; break; // Left edge
                        case 3: spawnPos = { (float)SCREEN_WIDTH + 50.0f, (float)GetRandomValue(0, SCREEN_HEIGHT) }; break; // Right edge
                    }

                    // Randomly spawn Fast or Tank zombies
                    if (GetRandomValue(0, 1) == 0)
                        zombies.emplace_back(FastZombie(spawnPos));
                    else
                        zombies.emplace_back(TankZombie(spawnPos));
                }

                // Update entities (player, zombies)
                player.update(deltaTime, zombies, walls);
                for (auto& zombie : zombies) {
                    zombie.update(player.pos, deltaTime, walls, player.health); // Passing player.health might be for zombie AI
                }

                // Remove dead zombies and update kill count
                int zombiesBeforeErase = (int)zombies.size();
                zombies.erase(std::remove_if(zombies.begin(), zombies.end(), [](const Zombie& z) { return z.isDead(); }), zombies.end());
                zombiesKilled += (zombiesBeforeErase - (int)zombies.size());

                // Check for floor completion
                if (zombiesKilled >= ZOMBIES_PER_FLOOR) {
                    if (currentFloor < MAX_FLOORS) {
                        // Advance to next floor
                        zombiesKilled = 0;
                        currentFloor++;
                        zombies.clear(); // Clear existing zombies
                        walls = CreateWalls(14); // Generate new walls for the next floor
                        spawnInterval *= 0.9f; // Make next floor harder (faster spawns)
                        if (spawnInterval < 0.5f) spawnInterval = 0.5f; // Cap minimum spawn interval
                        // Replenish player health slightly for reaching a new floor
                        player.health = ClampFloat(player.health + 20, 0, 100);
                    } else {
                        gameState = GAME_WIN; // All floors cleared, player wins
                    }
                }

                // Draw game elements
                for (const auto& wall : walls) {
                    // Wall Design Improvement: Add a subtle shadow and inner detail for a more realistic look
                    float shadowOffset = 4.0f;
                    float detailInset = 5.0f; // For inner detail

                    // Draw shadow (slightly offset, darker color)
                    DrawRectangleRounded({wall.x + shadowOffset, wall.y + shadowOffset, wall.width, wall.height}, 0.3f, 5, ColorAlpha(BLACK, 0.5f));

                    // Draw main wall body (a grungier, brownish-grey for concrete/stone)
                    DrawRectangleRounded(wall, 0.3f, 5, Color{90, 80, 70, 255}); 

                    // Draw inner detail (simulates bricks/texture or lighter worn areas)
                    // Smaller rectangle slightly lighter
                    DrawRectangleRounded({wall.x + detailInset, wall.y + detailInset, wall.width - 2 * detailInset, wall.height - 2 * detailInset}, 0.2f, 5, Color{110, 100, 90, 255});

                    // Draw a subtle dark border to define the shape more clearly
                    DrawRectangleRoundedLines(wall, 0.3f, 5, Color{60, 50, 40, 255}); 
                }

                player.draw(); // Draws player and their bullets

                for (const auto& zombie : zombies)
                    zombie.draw();

                // Draw the improved in-game HUD
                DrawGameHUD(player, currentFloor, MAX_FLOORS, zombiesKilled, ZOMBIES_PER_FLOOR);

                // Check for game over condition
                if (player.health <= 0) {
                    gameState = GAME_OVER;
                }

                break;
            }

            case GAME_OVER:
                DrawGameOverScreen(); // Display game over screen
                if (IsKeyPressed(KEY_R)) {
                    InitializeGame(); // Reset game state
                    gameState = SELECTING_WEAPON; // Go back to weapon selection
                }
                break;

            case GAME_WIN:
                DrawGameWinScreen(); // Display game win screen
                if (IsKeyPressed(KEY_R)) {
                    InitializeGame(); // Reset game state
                    gameState = SELECTING_WEAPON; // Go back to weapon selection
                }
                break;
        }

        EndDrawing();
    }
    
    // ADDED: Close the audio device before closing the window
    CloseAudioDevice(); 
    CloseWindow();
    return 0;
}
