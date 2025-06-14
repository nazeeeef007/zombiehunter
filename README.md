🧟 Zombie Survival: A C++ Raylib Adventure
About the Game
Zombie Survival is a fast-paced, top-down 2D shooter developed in C++ using the Raylib library. Players are plunged into a relentless arena, battling waves of diverse zombies across multiple procedurally generated floors. The game emphasizes fluid movement, tactical weapon usage, and challenging combat, offering a clear progression system as players strive to clear all floors and achieve victory.

✨ Key Features
Dynamic Gameplay: Engage in intense, wave-based combat against an ever-increasing horde of zombies.

Procedural Map Generation: Each floor features a unique layout of walls, ensuring fresh challenges with every playthrough.

Multiple Weapon Types: Choose between a Pistol, Shotgun, and Rifle, each with distinct fire rates, bullet speeds, and damage, offering varied tactical approaches.

Player Abilities: Utilize a strategic dash ability for evasive maneuvers and benefit from passive health regeneration during lulls in combat.

Robust Game States: Seamless transitions between weapon selection, active gameplay, game over, and victory screens.

Intuitive UI/HUD: A clear in-game display shows health, current weapon, floor progress, and zombie kill count.

Responsive Controls: Fluid player movement and precise aiming via mouse input.

Immersive Audio: Dynamic firing sounds for each weapon type, enhancing the combat experience.

🚀 Technical Deep Dive
This project demonstrates proficiency in several advanced C++ concepts and game development techniques, crucial for building performant and maintainable applications.

Core Architecture & Technologies
C++ & Raylib: The entire game logic and rendering are built from scratch in C++, leveraging the lightweight and powerful Raylib library for graphics, audio, input, and window management. This choice highlights a strong understanding of low-level programming and direct hardware interaction.

Object-Oriented Design (OOD): The game is structured using a robust OOD approach with clear responsibilities for key entities:

Player: Manages player state, input, abilities (dash, health regen), and bullet firing.

Weapon: Encapsulates weapon-specific properties (fire rate, damage, bullet speed) and manages its unique audio resources.

Bullet: Handles projectile movement, collision, and lifetime.

Zombie: Base class for enemy types, with specialized subclasses (e.g., FastZombie, TankZombie) demonstrating polymorphism.

Modular Design: Code is organized into multiple header (.h) and source (.cpp) files (Player.h/.cpp, Weapon.h/.cpp, WeaponTypes.h/.cpp, Zombie.h/.cpp, CollisionUtils.h/.cpp), promoting code reusability, maintainability, and efficient compilation.

Advanced C++ Concepts & Resource Management
Rule of Five (Move Semantics): A critical aspect of resource management is the correct handling of Raylib's Sound objects within the Weapon class. To prevent common C++ pitfalls like double-free errors and memory leaks (which often lead to crashes), the Weapon class explicitly implements:

A custom Destructor (~Weapon()) to call UnloadSound().

Deleted Copy Constructor and Copy Assignment Operator (= delete) to prevent shallow copies of the Sound resource.

Custom Move Constructor (Weapon(Weapon&&) noexcept) and Move Assignment Operator (Weapon& operator=(Weapon&&) noexcept) to efficiently transfer ownership of the Sound resource. This ensures that Sound data is properly managed when Weapon objects are created, passed to functions (like the Player constructor), or reassigned.

Procedural Content Generation: The game dynamically generates wall layouts for each new floor, showcasing an understanding of procedural algorithms for game content.

Collision Detection: Custom collision utilities (CollisionUtils.h/.cpp) are used for accurate player-wall, bullet-zombie, and bullet-wall interactions.

Build & Deployment
Cross-Platform Compilation (MinGW/G++): The game is compiled using the G++ compiler on Windows, demonstrating familiarity with standard C++ build processes.

WebAssembly (Wasm) Deployment: A significant technical achievement is the ability to compile the entire C++ application to WebAssembly using Emscripten. This allows the native C++ game to run directly in a web browser, enabling broad accessibility without requiring platform-specific downloads. This demonstrates expertise in cross-compilation and modern web technologies.

💻 How to Run
Native (Windows/Linux/macOS - Raylib compatible)
Clone the repository:

git clone https://github.com/YourUsername/zombie-survival.git
cd zombie-survival/src

Ensure Raylib is installed: Follow Raylib's installation guide for your specific OS.

Place Audio Assets: Create an assets/audio/ directory one level above your src directory (i.e., zombie-survival/assets/audio/) and place pistol_fire.wav, shotgun_fire.wav, and rifle_fire.wav files inside it.

Compile the game:

g++ main.cpp Player.cpp Zombie.cpp Weapon.cpp CollisionUtils.cpp WeaponTypes.cpp -o game.exe -lraylib -lopengl32 -lgdi32 -lwinmm

(Note: For Linux/macOS, library flags might vary, typically just -lraylib).

Run the executable:

./game.exe

Web (Browser)
Install Emscripten: Follow the official Emscripten SDK documentation.

Navigate to the src directory:

cd zombie-survival/src

Compile to WebAssembly:

emcc main.cpp Player.cpp Zombie.cpp Weapon.cpp CollisionUtils.cpp WeaponTypes.cpp -o index.html -s USE_GLFW=3 -s USE_WEBGL2=1 -s WASM=1 -s EXPORT_ES6=1 -s MODULARIZE=1 -s FORCE_FILESYSTEM=1 --preload-file ../assets@assets --embed-file ../assets/audio

This command compiles your C++ code to index.html, index.js, and index.wasm. The --preload-file ../assets@assets argument ensures your assets folder (including audio) is bundled into the web build.

Serve Locally: Due to browser security restrictions (CORS), you need a local web server to run the index.html file. You can use Python:

python -m http.server 8000

Open in Browser: Navigate to http://localhost:8000/index.html in your web browser.

💡 Future Enhancements
More diverse zombie types with unique behaviors and abilities.

Additional weapon variety, including melee weapons and grenades.

Power-ups and pickups to enhance gameplay.

Score tracking and a high-score system.

Environmental hazards or interactive elements.

Improved visual effects (particles, animations).
