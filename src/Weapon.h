#pragma once

#include "raylib.h"
#include "Player.h"
#include "Level.h"

// What a shot hit, so the HUD can show a hit marker.
enum class ShotResult { Missed, HitWorld, HitTarget, KilledTarget };

// A placeholder automatic rifle. Swap the drawing for a real 3D model later.
struct Weapon {
    int ammo = 0;
    int reserveAmmo = 0;
    float cooldown = 0;       // time until we can fire again
    bool reloading = false;
    float reloadTimer = 0;
    float kick = 0;           // visual recoil on the gun model
    float adsAmount = 0;      // 0 = hip fire, 1 = fully aimed

    void reset();
    void update(float dt, bool aiming, bool reloadPressed);
    bool canFire() const;
    ShotResult fire(Player& player, Level& level);

    void drawViewModel(int screenWidth, int screenHeight) const;
};
