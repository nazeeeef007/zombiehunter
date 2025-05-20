#include "raylib.h"
#include "Player.h"
#include "Weapon.h"
#include "ZombieTypes.h"
#include "WeaponTypes.h"
#include <algorithm>
#include "raymath.h"
#include <vector>
#include <memory>
#include <cstdlib>
#include <ctime>
#include "CollisionUtils.h"

const int SCREEN_WIDTH = 1200;
const int SCREEN_HEIGHT = 1000;

const int MAX_FLOORS = 3;
const int ZOMBIES_PER_FLOOR = 20;

float ClampFloat(float val, float minVal, float maxVal) {
    return (val < minVal) ? minVal : (val > maxVal) ? maxVal : val;
}

void SeedRandomWalls() {
    srand(static_cast<unsigned>(time(nullptr)));
}

std::vector<Rectangle> CreateWalls(int wallCount = 10) {
    std::vector<Rectangle> walls;

    for (int i = 0; i < wallCount; ++i) {
        float x = static_cast<float>(rand() % (SCREEN_WIDTH - 100));
        float y = static_cast<float>(rand() % (SCREEN_HEIGHT - 100));
        bool horizontal = rand() % 2;
        float width = horizontal ? static_cast<float>(100 + rand() % 200) : 20.0f;
        float height = horizontal ? 20.0f : static_cast<float>(100 + rand() % 200);
        walls.push_back({ x, y, width, height });
    }

    return walls;
}

enum GameState {
    SELECTING_WEAPON,
    PLAYING,
    GAME_OVER,
    GAME_WIN
};




void DrawWeaponSelectionScreen(float time, WeaponType& hoveredWeapon) {
    ClearBackground(DARKGRAY);
    DrawText("Choose Your Weapon", SCREEN_WIDTH / 2 - 170, 50, 40, YELLOW);

    Rectangle pistolRect = {150, 200, 250, 300};
    Rectangle shotgunRect = {475, 200, 250, 300};
    Rectangle rifleRect = {800, 200, 250, 300};

    // Detect Hover
    Vector2 mouse = GetMousePosition();
    hoveredWeapon = WeaponType::Pistol;
    if (CheckCollisionPointRec(mouse, pistolRect)) hoveredWeapon = WeaponType::Pistol;
    if (CheckCollisionPointRec(mouse, shotgunRect)) hoveredWeapon = WeaponType::Shotgun;
    if (CheckCollisionPointRec(mouse, rifleRect)) hoveredWeapon = WeaponType::Rifle;
  
    float animOffset = sinf(time * 4) * 10; // bounce animation

    // Draw Card UI with highlight if hovered
    auto DrawCard = [&](Rectangle rect, Color baseColor, const char* name, const char* stats[], int statCount, void (*DrawIcon)(int, int), bool selected) {
        DrawRectangleRounded(rect, 0.1f, 10, Fade(baseColor, selected ? 0.8f : 0.5f));
        DrawIcon(rect.x + 30, rect.y + 50);
        DrawText(name, rect.x + 30, rect.y + 20, 25, WHITE);
        for (int i = 0; i < statCount; ++i)
            DrawText(stats[i], rect.x + 30, rect.y + 90 + i * 30, 20, LIGHTGRAY);
    };

    const char* pistolStats[] = {"Fire Rate: 3", "Speed: 600", "Damage: 20"};
    const char* shotgunStats[] = {"Fire Rate: 1", "Speed: 400", "Damage: 50"};
    const char* rifleStats[] = {"Fire Rate: 5", "Speed: 800", "Damage: 15"};

    DrawCard(pistolRect, BLUE, "1. Pistol", pistolStats, 3, DrawPistolIcon, hoveredWeapon == WeaponType::Pistol);
    DrawCard(shotgunRect, RED, "2. Shotgun", shotgunStats, 3, DrawShotgunIcon, hoveredWeapon == WeaponType::Shotgun);
    DrawCard(rifleRect, GREEN, "3. Rifle", rifleStats, 3, DrawRifleIcon, hoveredWeapon == WeaponType::Rifle);

    // Preview Stickman + Weapon
    Vector2 previewPos = {SCREEN_WIDTH / 2, 550 + animOffset};

    // Stickman body
    DrawCircleV(previewPos, 15, LIGHTGRAY); // head
    DrawLineV({previewPos.x, previewPos.y + 15}, {previewPos.x, previewPos.y + 60}, WHITE); // body
    DrawLineV({previewPos.x, previewPos.y + 30}, {previewPos.x - 15, previewPos.y + 45}, WHITE); // left arm
    DrawLineV({previewPos.x, previewPos.y + 30}, {previewPos.x + 15, previewPos.y + 45}, WHITE); // right arm
    DrawLineV({previewPos.x, previewPos.y + 60}, {previewPos.x - 10, previewPos.y + 80}, WHITE); // left leg
    DrawLineV({previewPos.x, previewPos.y + 60}, {previewPos.x + 10, previewPos.y + 80}, WHITE); // right leg

    // Spinning Weapon Preview
    float spinAngle = time * 360.0f; // 1 rotation per second

    Vector2 weaponPos = {previewPos.x + 20, previewPos.y + 30};
    Rectangle weaponBox = {weaponPos.x, weaponPos.y, 40, 6};

    switch (hoveredWeapon) {
        case WeaponType::Pistol:
            DrawRectanglePro(weaponBox, {0, 3}, spinAngle, Fade(BLUE, 0.8f));
            break;
        case WeaponType::Shotgun:
            DrawRectanglePro(weaponBox, {0, 3}, spinAngle, Fade(RED, 0.8f));
            break;
        case WeaponType::Rifle:
            DrawRectanglePro(weaponBox, {0, 3}, spinAngle, Fade(GREEN, 0.8f));
            break;
        default:
            // Idle pistol preview if no hover
            DrawRectanglePro(weaponBox, {0, 3}, spinAngle, Fade(GRAY, 0.5f));
    }
}

int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "🧟 Zombie Survival");
    SetTargetFPS(60);
    SeedRandomWalls();

    GameState gameState = SELECTING_WEAPON;

    Weapon selectedWeapon = CreatePistol(); // default
    Player player({ 400, 300 }, { 1, 0 }, 20.0f, 100, selectedWeapon);
    std::vector<Rectangle> walls = CreateWalls(14);
    std::vector<Zombie> zombies;

    float spawnTimer = 0.0f;
    float spawnInterval = 2.0f;

    int currentFloor = 1;
    int zombiesKilled = 0;

    float curtime = 0.0f;
    WeaponType hoveredWeapon = WeaponType::Pistol;

    srand(static_cast<unsigned>(time(nullptr)));

    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();
        curtime += deltaTime;
        BeginDrawing();

        switch (gameState) {
            case SELECTING_WEAPON:
                DrawWeaponSelectionScreen(curtime, hoveredWeapon);

                if (IsKeyPressed(KEY_ONE)) {
                    selectedWeapon = CreatePistol();
                    gameState = PLAYING;
                    player.weapon = selectedWeapon;
                }
                if (IsKeyPressed(KEY_TWO)) {
                    selectedWeapon = CreateShotgun();
                    gameState = PLAYING;
                    player.weapon = selectedWeapon;
                }
                if (IsKeyPressed(KEY_THREE)) {
                    selectedWeapon = CreateRifle();
                    gameState = PLAYING;
                    player.weapon = selectedWeapon;
                }
                break;

            case PLAYING: {
                ClearBackground(Color{ 30, 30, 30, 255 });

                // Input
                Vector2 move = { 0, 0 };
                if (IsKeyDown(KEY_W)) move.y -= 1;
                if (IsKeyDown(KEY_S)) move.y += 1;
                if (IsKeyDown(KEY_D)) move.x += 1;
                if (IsKeyDown(KEY_A)) move.x -= 1;
                if (Vector2Length(move) > 0) move = Vector2Normalize(move);

                float moveSpeed = 200.0f;
                Vector2 newPos = Vector2Add(player.pos, Vector2Scale(move, moveSpeed * deltaTime));

                if (!CollidesWithWallCircle(newPos, player.size, walls)) {
                    player.pos = newPos;
                }

                // Facing
                Vector2 mouse = GetMousePosition();
                player.facing = Vector2Normalize(Vector2Subtract(mouse, player.pos));

                if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
                    player.shoot(GetTime());
                }

                // Spawn Zombies
                spawnTimer += deltaTime;
                if (spawnTimer >= spawnInterval && (int)zombies.size() < ZOMBIES_PER_FLOOR) {
                    spawnTimer = 0.0f;
                    Vector2 spawnPos;
                    int side = rand() % 4;
                    switch (side) {
                        case 0: spawnPos = { (float)(rand() % SCREEN_WIDTH), -30 }; break;
                        case 1: spawnPos = { (float)(rand() % SCREEN_WIDTH), SCREEN_HEIGHT + 30 }; break;
                        case 2: spawnPos = { -30, (float)(rand() % SCREEN_HEIGHT) }; break;
                        case 3: spawnPos = { SCREEN_WIDTH + 30, (float)(rand() % SCREEN_HEIGHT) }; break;
                    }
                    if (rand() % 2 == 0)
                        zombies.emplace_back(FastZombie(spawnPos));
                    else
                        zombies.emplace_back(TankZombie(spawnPos));
                }

                // Update entities
                player.update(deltaTime, zombies, walls);
                for (auto& zombie : zombies) {
                    zombie.update(player.pos, deltaTime, walls, player.health);
                }

                int before = (int)zombies.size();
                zombies.erase(std::remove_if(zombies.begin(), zombies.end(), [](const Zombie& z) { return z.isDead(); }), zombies.end());
                zombiesKilled += (before - (int)zombies.size());

                // Check for floor completion
                if (zombiesKilled >= ZOMBIES_PER_FLOOR) {
                    zombiesKilled = 0;
                    currentFloor++;
                    zombies.clear();
                    if (currentFloor > MAX_FLOORS) {
                        gameState = GAME_WIN;
                    }
                }

                // Draw
                DrawCircleGradient((int)player.pos.x, (int)player.pos.y, 120, Color{ 40, 40, 40, 100 }, BLACK);
                for (const auto& wall : walls)
                    DrawRectangleRounded(wall, 0.3f, 5, DARKGRAY);

                player.draw();

                for (const auto& zombie : zombies)
                    zombie.draw();

                DrawRectangle(0, 0, SCREEN_WIDTH, 60, Fade(BLACK, 0.5f));
                DrawText("Zombie Survival", 20, 10, 25, GREEN);
                DrawText(TextFormat("Floor: %d/%d", currentFloor, MAX_FLOORS), 250, 15, 20, LIGHTGRAY);
                DrawText(TextFormat("Zombies Killed: %d/%d", zombiesKilled, ZOMBIES_PER_FLOOR), 450, 15, 20, ORANGE);

                DrawCircleLines((int)mouse.x, (int)mouse.y, 8, RED);
                DrawLine((int)mouse.x - 10, (int)mouse.y, (int)mouse.x + 10, (int)mouse.y, RED);
                DrawLine((int)mouse.x, (int)mouse.y - 10, (int)mouse.x, (int)mouse.y + 10, RED);

                if (player.health <= 0) {
                    gameState = GAME_OVER;
                }

                break;
            }

            case GAME_OVER:
                ClearBackground(BLACK);
                DrawText("💀 GAME OVER 💀", SCREEN_WIDTH / 2 - 170, SCREEN_HEIGHT / 2 - 10, 40, RED);
                break;

            case GAME_WIN:
                ClearBackground(DARKGREEN);
                DrawText("🏆 ALL FLOORS CLEARED! YOU WIN!", SCREEN_WIDTH / 2 - 250, SCREEN_HEIGHT / 2 - 10, 40, YELLOW);
                break;
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
