# FPS Prototype

A first-person shooter starter written in C++ with [raylib](https://www.raylib.com/).
Everything is placeholder boxes for now; real models and maps come later.

## What's in it

- **Movement:** walk, sprint, jump, crouch, slide (crouch while sprinting), wall slide and wall jump
- **Weapon:** placeholder automatic rifle with recoil, spread, aim down sights, reload, muzzle flash
- **Test level:** floor, walls, crates, a low overhang to slide under, two tall walls for wall sliding, and 5 shooting-range targets that respawn
- **HUD:** crosshair that spreads when you move, hit markers, ammo count, movement state and speed

## Controls

| Key | Action |
| --- | --- |
| WASD | Move |
| Mouse | Look |
| Shift | Sprint |
| Space | Jump (also wall jump while wall sliding) |
| C or Left Ctrl | Crouch, or slide while sprinting |
| Left mouse | Shoot |
| Right mouse | Aim down sights |
| R | Reload |
| Esc | Quit |

To wall slide: jump at one of the tall grey walls and hold the key that moves you into it. Press Space to kick off.

## Building

You need a C++ compiler and CMake. raylib downloads itself the first time you build.

**Windows (Visual Studio):**
1. Open Visual Studio and choose **Open a local folder**, then pick this repo's folder
2. Wait for the CMake configure to finish at the bottom of the window (the first time takes a minute while raylib downloads)
3. In the green play button's dropdown at the top, choose **fps_game.exe**, then press it

**Command line (any system):**

```
cmake -S . -B build
cmake --build build
```

## Where things live

| File | What it does |
| --- | --- |
| `src/main.cpp` | Opens the window and runs the game loop: input, update, draw |
| `src/Config.h` | Every number that controls the feel: speeds, gravity, fire rate, damage |
| `src/Player.h/.cpp` | Movement, the movement states, and collision with the level |
| `src/Weapon.h/.cpp` | Firing, recoil, spread, reloading, and the placeholder gun drawing |
| `src/Level.h/.cpp` | The test map, the targets, and bullet impact marks |
| `src/Hud.h/.cpp` | Crosshair, hit markers, ammo and debug text |
