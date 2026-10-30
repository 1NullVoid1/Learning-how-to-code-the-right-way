#include "raylib.h"
#include "raymath.h"
#include "Config.h"
#include "Level.h"
#include "Player.h"
#include "Weapon.h"
#include "Hud.h"

// Read the keyboard and mouse once per frame.
static PlayerInput readInput() {
    PlayerInput input;
    if (IsKeyDown(KEY_W)) input.forward += 1;
    if (IsKeyDown(KEY_S)) input.forward -= 1;
    if (IsKeyDown(KEY_D)) input.right += 1;
    if (IsKeyDown(KEY_A)) input.right -= 1;
    input.look = GetMouseDelta();
    input.jump = IsKeyPressed(KEY_SPACE);
    input.crouch = IsKeyDown(KEY_C) || IsKeyDown(KEY_LEFT_CONTROL);
    input.aiming = IsMouseButtonDown(MOUSE_BUTTON_RIGHT);

    // Shooting or aiming cancels sprint, like most shooters
    bool shooting = IsMouseButtonDown(MOUSE_BUTTON_LEFT);
    input.sprint = IsKeyDown(KEY_LEFT_SHIFT) && !shooting && !input.aiming;
    return input;
}

int main() {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "FPS Prototype");
    SetTargetFPS(144);
    DisableCursor();   // lock and hide the mouse for looking around

    Level level;
    level.build();

    Player player;
    player.spawn({ 0, 1, 0 }, 0);

    Weapon weapon;
    weapon.reset();

    Hud hud;

    Camera3D camera = {};
    camera.up = { 0, 1, 0 };
    camera.fovy = HIP_FOV;
    camera.projection = CAMERA_PERSPECTIVE;

    // ---------------- The game loop ----------------
    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        if (dt > 0.05f) dt = 0.05f;   // avoid huge jumps if the game hitches

        // 1. Input
        PlayerInput input = readInput();
        bool triggerHeld = IsMouseButtonDown(MOUSE_BUTTON_LEFT);
        bool reloadPressed = IsKeyPressed(KEY_R);

        // 2. Update
        player.update(input, level, dt);
        bool aiming = input.aiming && player.state != MoveState::Sprinting;
        weapon.update(dt, aiming, reloadPressed);
        if (triggerHeld && weapon.canFire() && player.state != MoveState::Sprinting) {
            ShotResult result = weapon.fire(player, level);
            hud.onShot(result);
        }
        level.update(dt);
        hud.update(dt);

        camera.position = player.getEyePosition();
        camera.target = Vector3Add(camera.position, player.getLookDirection());
        camera.fovy = Lerp(HIP_FOV, ADS_FOV, weapon.adsAmount);

        // 3. Draw
        BeginDrawing();
        ClearBackground(SKYBLUE);

        BeginMode3D(camera);
        level.draw();
        EndMode3D();

        weapon.drawViewModel(GetScreenWidth(), GetScreenHeight());
        hud.draw(player, weapon, GetScreenWidth(), GetScreenHeight());

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
