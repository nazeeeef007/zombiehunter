# 🧟 Zombie Survival: A C++ Raylib Adventure

---

## 🎮 About the Game

*Zombie Survival* is a fast-paced, top-down 2D shooter built with C++ and Raylib.  
Battle endless waves of zombies across procedurally generated floors in an intense arena.  
Master fluid movement, strategic weapon use, and tough combat to clear all floors and claim victory!

---

## ✨ Key Features

- **Dynamic Wave Combat:** Face growing hordes of zombies with unique behaviors.  
- **Procedural Maps:** Every floor has a fresh, randomly generated layout of walls.  
- **Varied Weapons:**  
  - *Pistol* — balanced fire rate and damage  
  - *Shotgun* — high damage, short range  
  - *Rifle* — rapid fire, long range  
- **Player Abilities:** Dash for evasive maneuvers and regenerate health during lulls.  
- **Smooth Game States:** Weapon selection, gameplay, game over, and victory screens.  
- **Clear UI/HUD:** Shows health, weapon, floor progress, and zombie kills.  
- **Responsive Controls:** Mouse aiming + smooth player movement.  
- **Immersive Audio:** Unique sounds for each weapon add depth to combat.

---

## 🚀 Technical Highlights

### Core Technologies  
- **Language & Library:** C++ with [Raylib](https://www.raylib.com) for graphics, input, audio, and window management.  
- **OOP Design:**  
  - `Player`: handles input, state, dash, shooting  
  - `Weapon`: encapsulates fire rate, damage, bullet speed, and sound resources  
  - `Bullet`: manages movement, collisions, lifetime  
  - `Zombie`: base + specialized enemy classes (e.g., `FastZombie`, `TankZombie`)  
- **Modular Codebase:** Clean separation across header (`.h`) and source (`.cpp`) files.

### Advanced C++ Concepts  
- **Rule of Five:**  
  - Safe management of Raylib `Sound` resources inside `Weapon` class.  
  - Custom destructor, deleted copy constructor/assignment, and implemented move semantics prevent leaks & crashes.

### Gameplay Mechanics  
- Procedural wall layouts for each floor.  
- Custom collision detection between player, zombies, bullets, and walls.

### Build & Deployment  
- Cross-platform compilation with `g++` (Windows/Linux/macOS).  
- WebAssembly build with Emscripten for browser play — no downloads required!

---

## 💻 How to Run

### Native (Windows/Linux/macOS)

```bash
git clone https://github.com/YourUsername/zombie-survival.git
cd zombie-survival/src
