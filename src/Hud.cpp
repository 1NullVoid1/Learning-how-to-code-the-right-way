#include "Hud.h"
#include "Config.h"

void Hud::onShot(ShotResult result) {
    if (result == ShotResult::HitTarget || result == ShotResult::KilledTarget) {
        hitMarkerTimer = 0.15f;
        lastHitWasKill = (result == ShotResult::KilledTarget);
    }
}

void Hud::update(float dt) {
    if (hitMarkerTimer > 0) {
        hitMarkerTimer -= dt;
    }
}

void Hud::draw(const Player& player, const Weapon& weapon, int screenWidth, int screenHeight) const {
    int cx = screenWidth / 2;
    int cy = screenHeight / 2;

    // Crosshair: four lines that spread apart when you move, hidden when aiming
    if (weapon.adsAmount < 0.5f) {
        int gap = 6 + (int)(player.getHorizontalSpeed() * 1.5f);
        if (!player.grounded) gap += 10;
        DrawRectangle(cx - gap - 10, cy - 1, 10, 2, WHITE);
        DrawRectangle(cx + gap, cy - 1, 10, 2, WHITE);
        DrawRectangle(cx - 1, cy - gap - 10, 2, 10, WHITE);
        DrawRectangle(cx - 1, cy + gap, 2, 10, WHITE);
    } else {
        DrawCircle(cx, cy, 2, RED);   // red dot sight
    }

    // Hit marker: an X when you hit a target (red on a kill)
    if (hitMarkerTimer > 0) {
        Color color = lastHitWasKill ? RED : WHITE;
        DrawLineEx({ (float)cx - 12, (float)cy - 12 }, { (float)cx - 5, (float)cy - 5 }, 2, color);
        DrawLineEx({ (float)cx + 12, (float)cy - 12 }, { (float)cx + 5, (float)cy - 5 }, 2, color);
        DrawLineEx({ (float)cx - 12, (float)cy + 12 }, { (float)cx - 5, (float)cy + 5 }, 2, color);
        DrawLineEx({ (float)cx + 12, (float)cy + 12 }, { (float)cx + 5, (float)cy + 5 }, 2, color);
    }

    // Ammo, bottom right
    DrawText(TextFormat("%d / %d", weapon.ammo, weapon.reserveAmmo),
             screenWidth - 180, screenHeight - 50, 30, WHITE);
    if (weapon.reloading) {
        DrawText("RELOADING", cx - 60, cy + 40, 20, YELLOW);
    } else if (weapon.ammo == 0 && weapon.reserveAmmo == 0) {
        DrawText("OUT OF AMMO", cx - 70, cy + 40, 20, RED);
    }

    // Debug info, top left: handy while tuning movement
    DrawFPS(10, 10);
    DrawText(TextFormat("State: %s", moveStateName(player.state)), 10, 35, 20, WHITE);
    DrawText(TextFormat("Speed: %.1f m/s", player.getHorizontalSpeed()), 10, 60, 20, WHITE);

    // Controls, bottom left
    DrawText("WASD move | Shift sprint | Space jump | C / Ctrl crouch & slide", 10, screenHeight - 50, 16, RAYWHITE);
    DrawText("Mouse look | LMB shoot | RMB aim | R reload | Esc quit", 10, screenHeight - 28, 16, RAYWHITE);
}
