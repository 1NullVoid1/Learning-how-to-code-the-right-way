#include "Weapon.h"
#include "Config.h"
#include "raymath.h"

const float TARGET_RESPAWN_TIME = 3.0f;

void Weapon::reset() {
    ammo = MAG_SIZE;
    reserveAmmo = START_RESERVE_AMMO;
    cooldown = 0;
    reloading = false;
    reloadTimer = 0;
    kick = 0;
    adsAmount = 0;
}

void Weapon::update(float dt, bool aiming, bool reloadPressed) {
    cooldown -= dt;
    kick = Lerp(kick, 0.0f, Clamp(10.0f * dt, 0.0f, 1.0f));
    adsAmount = Lerp(adsAmount, aiming ? 1.0f : 0.0f, Clamp(12.0f * dt, 0.0f, 1.0f));

    // Start a reload: pressed R, or the mag is empty
    bool wantsReload = reloadPressed || ammo == 0;
    if (wantsReload && !reloading && ammo < MAG_SIZE && reserveAmmo > 0) {
        reloading = true;
        reloadTimer = RELOAD_TIME;
    }

    if (reloading) {
        reloadTimer -= dt;
        if (reloadTimer <= 0) {
            int needed = MAG_SIZE - ammo;
            int taken = (needed < reserveAmmo) ? needed : reserveAmmo;
            ammo += taken;
            reserveAmmo -= taken;
            reloading = false;
        }
    }
}

bool Weapon::canFire() const {
    return ammo > 0 && cooldown <= 0 && !reloading;
}

ShotResult Weapon::fire(Player& player, Level& level) {
    ammo--;
    cooldown = FIRE_INTERVAL;
    kick = 1.0f;
    player.addRecoil(RECOIL_KICK * (1.0f - 0.5f * adsAmount));

    // Spread: aim gets wobbly from the hip, tight when aiming
    float spread = Lerp(HIP_SPREAD, ADS_SPREAD, adsAmount);
    float randomX = GetRandomValue(-100, 100) / 100.0f * spread;
    float randomY = GetRandomValue(-100, 100) / 100.0f * spread;
    Vector3 up = { 0, 1, 0 };
    Vector3 direction = player.getLookDirection();
    direction = Vector3Add(direction, Vector3Scale(player.getRight(), randomX));
    direction = Vector3Add(direction, Vector3Scale(up, randomY));
    direction = Vector3Normalize(direction);

    Ray ray = { player.getEyePosition(), direction };

    // Find the closest thing the bullet hits (a "hitscan" weapon)
    float closest = WEAPON_RANGE;
    Target* hitTarget = nullptr;
    Vector3 hitPoint = { 0, 0, 0 };
    bool hitSomething = false;

    for (const Block& block : level.blocks) {
        RayCollision hit = GetRayCollisionBox(ray, block.box);
        if (hit.hit && hit.distance < closest) {
            closest = hit.distance;
            hitPoint = hit.point;
            hitTarget = nullptr;
            hitSomething = true;
        }
    }
    for (Target& target : level.targets) {
        if (!target.isAlive()) continue;
        RayCollision hit = GetRayCollisionBox(ray, target.getBox());
        if (hit.hit && hit.distance < closest) {
            closest = hit.distance;
            hitPoint = hit.point;
            hitTarget = &target;
            hitSomething = true;
        }
    }

    if (!hitSomething) {
        return ShotResult::Missed;
    }

    if (hitTarget != nullptr) {
        hitTarget->health -= WEAPON_DAMAGE;
        if (!hitTarget->isAlive()) {
            hitTarget->respawnTimer = TARGET_RESPAWN_TIME;
            return ShotResult::KilledTarget;
        }
        return ShotResult::HitTarget;
    }

    level.impacts.push_back({ hitPoint, IMPACT_LIFETIME });
    return ShotResult::HitWorld;
}

// A simple 2D placeholder gun in the bottom-right corner.
// It moves to the center when aiming and jumps back on each shot.
void Weapon::drawViewModel(int screenWidth, int screenHeight) const {
    float hipX = screenWidth * 0.70f;
    float adsX = screenWidth * 0.5f - 40;
    float x = Lerp(hipX, adsX, adsAmount);
    float y = screenHeight - 170 + kick * 25;
    if (reloading) {
        y += 120;   // drop the gun out of view while reloading
    }

    DrawRectangle((int)x, (int)y, 80, 200, DARKGRAY);              // body
    DrawRectangle((int)x + 25, (int)y - 70, 30, 80, GRAY);         // barrel
    DrawRectangle((int)x + 20, (int)y + 40, 40, 60, BLACK);        // magazine
    if (kick > 0.6f) {
        DrawCircle((int)x + 40, (int)y - 80, 18, YELLOW);          // muzzle flash
    }
}
