#pragma once

#include "raylib.h"
#include <vector>

// A solid box in the world: floor, walls, crates.
struct Block {
    BoundingBox box;
    Color color;
};

// A shooting-range dummy. Respawns a few seconds after it's destroyed.
struct Target {
    Vector3 position;   // bottom-center
    float health;
    float respawnTimer;

    bool isAlive() const { return health > 0.0f; }
    BoundingBox getBox() const;
};

// A little mark left where a bullet hit something.
struct Impact {
    Vector3 position;
    float timeLeft;
};

struct Level {
    std::vector<Block> blocks;
    std::vector<Target> targets;
    std::vector<Impact> impacts;

    void build();               // creates the placeholder test map
    void update(float dt);      // respawns targets, fades impacts
    void draw() const;
};
