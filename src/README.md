# 🧟‍♂️ Zombie Hunter

Play it live: [https://zombiehunter2.netlify.app](https://zombiehunter2.netlify.app)

A fast-paced 2D zombie survival shooter built **entirely from scratch** without using any game engine — just **C++**, **WebAssembly (WASM)**, and **raw HTML/JavaScript glue code**. You play as a lone survivor, taking on waves of relentless zombies with only your reflexes and firepower to survive. Think DOOM meets Plants vs Zombies, but coded at the byte level.

---

## 🎮 Gameplay

- **Top-down shooter**: Navigate with WASD keys, aim and shoot with your mouse.
- **Endless waves**: Zombies increase in number and difficulty as time progresses.
- **Power-ups**: Pick up ammo and health to stay alive longer.
- **Survival score**: Track your kills and how long you last.

---

## 🛠️ Technical Overview

### ✅ No Game Engine

Unlike many browser games that rely on Unity, Godot, or Phaser, **Zombie Hunter** is powered by **bare-metal logic** and a custom game loop. This makes the game lightweight, fast, and educational.

### ⚙️ C++ → WebAssembly

- The entire game logic was written in **modern C++**, taking full advantage of object-oriented design and memory safety.
- Then, it was compiled to **WebAssembly (WASM)** using **Emscripten**, making the native performance portable to the web.
- A lightweight **HTML + JS bridge** handles rendering and user input, forwarding actions to the WASM core in real-time.

### 💡 Why WebAssembly?

- Near-native performance in the browser
- Smaller binary footprint compared to JS
- A powerful way to port native C++ apps and games to the web without rewriting the logic

---

## 🚀 Deployment

- Hosted via [Netlify](https://www.netlify.com) using a custom subdomain:  
  **https://zombiehunter.netlify.app**
- Fully static, no backend required
- Loads instantly thanks to minimal dependencies

---

## 💭 Challenges Faced

- 🚫 No high-level framework or rendering engine — everything from drawing to collision was manually coded.
- 🧩 WebAssembly debugging can be tricky — had to handle memory layout, JS ↔ C++ communication, and browser quirks.
- 🔁 Porting from a native C++ app required adapting input models, memory handling, and rendering logic to the browser environment.

---

## 🧠 Lessons Learned

This project was an exercise in:

- Game loop and frame timing precision
- Memory management in a constrained environment
- Optimizing for performance in WASM vs native
- JavaScript ↔ WebAssembly interop
- Deploying C++-based games to the web

---
