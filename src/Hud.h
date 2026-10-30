#pragma once

#include "Player.h"
#include "Weapon.h"

struct Hud {
    float hitMarkerTimer = 0;
    bool lastHitWasKill = false;

    void onShot(ShotResult result);
    void update(float dt);
    void draw(const Player& player, const Weapon& weapon, int screenWidth, int screenHeight) const;
};
