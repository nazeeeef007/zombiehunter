🧟 Zombie Survival: A C++ Raylib Adventure
About the Game
Zombie Survival is a fast-paced, top-down 2D shooter developed in C++ using the lightweight Raylib library. Dive into a relentless arena where you fight waves of diverse zombies across procedurally generated floors. With fluid controls, tactical weapons, and challenging combat, players experience a thrilling progression system as they battle to clear all floors and achieve victory.

✨ Key Features
Dynamic Wave-Based Combat: Face off against ever-increasing hordes of zombies with varied behaviors.

Procedural Map Generation: Each floor presents a unique, randomly generated layout to keep every run fresh.

Multiple Weapons: Choose between Pistol, Shotgun, and Rifle — each with distinct fire rates, bullet speeds, and damage.

Player Abilities: Use a strategic dash to evade enemies and enjoy passive health regeneration when out of combat.

Robust Game States: Seamless transitions between weapon selection, active gameplay, game over, and victory screens.

Intuitive UI/HUD: Monitor your health, equipped weapon, floor progress, and zombie kills at a glance.

Responsive Controls: Smooth player movement with precise mouse aiming for an immersive experience.

Immersive Audio: Unique firing sounds for each weapon enhance the combat atmosphere.

🚀 Technical Overview
Core Architecture & Technologies
C++ & Raylib: Entire game logic and rendering are built from scratch in C++, leveraging Raylib for graphics, audio, input, and window management — demonstrating strong low-level programming skills.

Object-Oriented Design:

Player: Handles input, state, dash ability, health regen, and shooting.

Weapon: Manages weapon stats and audio resources with careful sound resource management.

Bullet: Controls projectile behavior, collision, and lifespan.

Zombie: Base enemy class with subclasses like FastZombie and TankZombie, showcasing polymorphism.

Modular Codebase: Organized into multiple .h and .cpp files (e.g., Player.h/.cpp, Weapon.h/.cpp, Zombie.h/.cpp, CollisionUtils.h/.cpp) for maintainability and scalability.

Advanced C++ Resource Management
Implements the Rule of Five to safely handle Raylib's Sound resources inside Weapon objects:

Custom destructor calls UnloadSound()

Copy constructor and copy assignment operator deleted to avoid shallow copies

Move constructor and move assignment operator implemented to transfer ownership safely and prevent memory leaks or crashes

Gameplay Mechanics
Procedural Content: Generates new wall layouts per floor using procedural algorithms.

Collision Detection: Custom utilities handle player-wall, bullet-zombie, and bullet-wall interactions accurately.

Build & Deployment
Cross-Platform Compilation: Uses g++ (MinGW) for Windows and supports Linux/macOS with minor flag changes.

WebAssembly Deployment: Compiled to WebAssembly with Emscripten, enabling native C++ gameplay directly in modern web browsers without downloads.

💻 How to Run
Native (Windows/Linux/macOS)
Clone the repo:

bash
Copy
Edit
git clone https://github.com/YourUsername/zombie-survival.git
cd zombie-survival/src
Install Raylib for your OS (Raylib installation guide).

Place audio files (pistol_fire.wav, shotgun_fire.wav, rifle_fire.wav) in assets/audio/ folder at the root level (parallel to src).

Compile:

bash
Copy
Edit
g++ main.cpp Player.cpp Zombie.cpp Weapon.cpp CollisionUtils.cpp WeaponTypes.cpp -o game.exe -lraylib -lopengl32 -lgdi32 -lwinmm
(Adjust library flags on Linux/macOS)

Run:

bash
Copy
Edit
./game.exe
Web (Browser)
Install Emscripten SDK.

Navigate to src:

bash
Copy
Edit
cd zombie-survival/src
Compile to WebAssembly:

bash
Copy
Edit
emcc main.cpp Player.cpp Zombie.cpp Weapon.cpp CollisionUtils.cpp WeaponTypes.cpp -o index.html -s USE_GLFW=3 -s USE_WEBGL2=1 -s WASM=1 -s EXPORT_ES6=1 -s MODULARIZE=1 -s FORCE_FILESYSTEM=1 --preload-file ../assets@assets --embed-file ../assets/audio
Serve locally (due to browser security restrictions):

bash
Copy
Edit
python -m http.server 8000
Open your browser at http://localhost:8000/index.html.

💡 Future Enhancements
More diverse zombie types with unique AI and abilities.

Additional weapons including melee and explosives.

Power-ups and pickups to enrich gameplay.

Score tracking and leaderboards.

Environmental hazards and interactive elements.

Enhanced visuals with particles and animations.

Thanks for checking out Zombie Survival! Feel free to contribute or open issues for bugs and feature requests.

