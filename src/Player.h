#pragma once

#include "raylib.h"
#include "Level.h"

// What the player is doing right now. Only one at a time.
enum class MoveState { Walking, Sprinting, Crouching, Sliding, Airborne, WallSliding };

const char* moveStateName(MoveState state);

// Everything the player pressed this frame, gathered in main.cpp.
// Keeping input separate makes it easy to add controller support later.
struct PlayerInput {
    float forward = 0;   // +1 = W, -1 = S
    float right = 0;     // +1 = D, -1 = A
    Vector2 look = { 0, 0 };   // mouse movement
    bool jump = false;
    bool sprint = false;
    bool crouch = false;
    bool aiming = false;
};

struct Player {
    Vector3 position = { 0, 0, 0 };   // feet
    Vector3 velocity = { 0, 0, 0 };
    float yaw = 0;      // looking left/right
    float pitch = 0;    // looking up/down
    float height = 0;   // shrinks when crouching
    MoveState state = MoveState::Walking;
    bool grounded = false;
    bool touchingWall = false;
    Vector3 wallNormal = { 0, 0, 0 };   // points away from the wall we touched

    void spawn(Vector3 spawnPosition, float spawnYaw);
    void update(const PlayerInput& input, const Level& level, float dt);
    void addRecoil(float amount);

    Vector3 getEyePosition() const;
    Vector3 getLookDirection() const;
    Vector3 getFlatForward() const;   // forward, ignoring up/down
    Vector3 getRight() const;
    float getHorizontalSpeed() const;

private:
    void updateState(const PlayerInput& input, const Level& level, Vector3 wishDir);
    void updateVelocity(const PlayerInput& input, Vector3 wishDir, float dt);
    void moveAndCollide(const Level& level, float dt);
    BoundingBox getBox(float boxHeight) const;
    bool canStandUp(const Level& level) const;
};
