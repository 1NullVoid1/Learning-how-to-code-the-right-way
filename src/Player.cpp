#include "Player.h"
#include "Config.h"
#include "raymath.h"
#include <cmath>

const char* moveStateName(MoveState state) {
    switch (state) {
        case MoveState::Walking:     return "Walking";
        case MoveState::Sprinting:   return "Sprinting";
        case MoveState::Crouching:   return "Crouching";
        case MoveState::Sliding:     return "Sliding";
        case MoveState::Airborne:    return "Airborne";
        case MoveState::WallSliding: return "Wall sliding";
    }
    return "?";
}

// Do two boxes overlap? (Just touching doesn't count.)
static bool overlaps(const BoundingBox& a, const BoundingBox& b) {
    return a.min.x < b.max.x && a.max.x > b.min.x &&
           a.min.y < b.max.y && a.max.y > b.min.y &&
           a.min.z < b.max.z && a.max.z > b.min.z;
}

void Player::spawn(Vector3 spawnPosition, float spawnYaw) {
    position = spawnPosition;
    velocity = { 0, 0, 0 };
    yaw = spawnYaw;
    pitch = 0;
    height = STAND_HEIGHT;
    state = MoveState::Airborne;
    grounded = false;
    touchingWall = false;
}

void Player::update(const PlayerInput& input, const Level& level, float dt) {
    // 1. Turn the camera with the mouse
    yaw -= input.look.x * MOUSE_SENSITIVITY;
    pitch -= input.look.y * MOUSE_SENSITIVITY;
    pitch = Clamp(pitch, -1.5f, 1.5f);   // don't flip upside down

    // 2. Which way do the keys want us to go? (flat on the ground)
    Vector3 wishDir = Vector3Add(Vector3Scale(getFlatForward(), input.forward),
                                 Vector3Scale(getRight(), input.right));
    if (Vector3Length(wishDir) > 0) {
        wishDir = Vector3Normalize(wishDir);
    }

    // 3. Decide what we're doing, then how fast we move, then actually move
    updateState(input, level, wishDir);
    updateVelocity(input, wishDir, dt);
    moveAndCollide(level, dt);

    // 4. Smoothly shrink or grow when crouching / standing
    bool low = (state == MoveState::Crouching || state == MoveState::Sliding);
    float targetHeight = low ? CROUCH_HEIGHT : STAND_HEIGHT;
    height = Lerp(height, targetHeight, Clamp(12.0f * dt, 0.0f, 1.0f));
}

void Player::updateState(const PlayerInput& input, const Level& level, Vector3 wishDir) {
    float speed = getHorizontalSpeed();

    if (grounded) {
        if (state == MoveState::Sliding) {
            // Keep sliding until we let go of crouch or slow down
            if (!input.crouch || speed < SLIDE_END_SPEED) {
                state = MoveState::Crouching;
            }
        } else if (input.crouch) {
            // Crouching while sprinting starts a slide
            if (state == MoveState::Sprinting && speed > WALK_SPEED) {
                Vector3 dir = Vector3Normalize({ velocity.x, 0, velocity.z });
                velocity.x = dir.x * SLIDE_BOOST;
                velocity.z = dir.z * SLIDE_BOOST;
                state = MoveState::Sliding;
            } else {
                state = MoveState::Crouching;
            }
        } else if (state == MoveState::Crouching && !canStandUp(level)) {
            // Something is above our head: stay crouched
        } else if (input.sprint && input.forward > 0 && !input.aiming) {
            state = MoveState::Sprinting;
        } else {
            state = MoveState::Walking;
        }

        // Jump (you keep your speed, so slide-jumping is fast!)
        bool lowCeiling = (height < STAND_HEIGHT - 0.1f) && !canStandUp(level);
        if (input.jump && !lowCeiling) {
            velocity.y = JUMP_SPEED;
            grounded = false;
            state = MoveState::Airborne;
        }
    } else {
        // In the air: wall slide if we're falling and pushing into a wall
        bool pushingIntoWall = Vector3DotProduct(wishDir, wallNormal) < -0.3f;
        if (touchingWall && velocity.y < 0 && pushingIntoWall) {
            state = MoveState::WallSliding;
        } else {
            state = MoveState::Airborne;
        }

        // Wall jump: kick off away from the wall
        if (state == MoveState::WallSliding && input.jump) {
            velocity.x = wallNormal.x * WALL_JUMP_PUSH;
            velocity.z = wallNormal.z * WALL_JUMP_PUSH;
            velocity.y = WALL_JUMP_UP;
            state = MoveState::Airborne;
        }
    }
}

void Player::updateVelocity(const PlayerInput& input, Vector3 wishDir, float dt) {
    Vector3 flatVel = { velocity.x, 0, velocity.z };
    float speed = Vector3Length(flatVel);

    if (state == MoveState::Sliding) {
        // Slides just slow down over time
        float newSpeed = fmaxf(speed - SLIDE_FRICTION * dt, 0.0f);
        if (speed > 0) {
            flatVel = Vector3Scale(flatVel, newSpeed / speed);
        }
    } else {
        float maxSpeed = WALK_SPEED;
        if (state == MoveState::Sprinting) maxSpeed = SPRINT_SPEED;
        if (state == MoveState::Crouching) maxSpeed = CROUCH_SPEED;
        if (input.aiming && grounded) maxSpeed = fminf(maxSpeed, ADS_SPEED);

        float accel = GROUND_ACCEL;
        if (!grounded) {
            accel = AIR_ACCEL;
            maxSpeed = fmaxf(speed, WALK_SPEED);   // keep momentum in the air
        }

        bool hasInput = Vector3Length(wishDir) > 0;
        if (grounded || hasInput) {
            // Move our velocity toward the speed and direction we want
            Vector3 target = Vector3Scale(wishDir, maxSpeed);
            Vector3 diff = Vector3Subtract(target, flatVel);
            float maxChange = accel * dt;
            if (Vector3Length(diff) > maxChange) {
                diff = Vector3Scale(Vector3Normalize(diff), maxChange);
            }
            flatVel = Vector3Add(flatVel, diff);
        }
    }

    velocity.x = flatVel.x;
    velocity.z = flatVel.z;

    // Gravity
    velocity.y -= GRAVITY * dt;
    if (state == MoveState::WallSliding && velocity.y < -WALL_SLIDE_FALL_SPEED) {
        velocity.y = -WALL_SLIDE_FALL_SPEED;
    }
}

// Move one axis at a time. If we end up inside a block, push back out.
void Player::moveAndCollide(const Level& level, float dt) {
    grounded = false;
    touchingWall = false;

    // X axis
    position.x += velocity.x * dt;
    for (const Block& block : level.blocks) {
        if (overlaps(getBox(height), block.box)) {
            if (velocity.x > 0) {
                position.x = block.box.min.x - PLAYER_RADIUS;
                wallNormal = { -1, 0, 0 };
            } else {
                position.x = block.box.max.x + PLAYER_RADIUS;
                wallNormal = { 1, 0, 0 };
            }
            velocity.x = 0;
            touchingWall = true;
        }
    }

    // Z axis
    position.z += velocity.z * dt;
    for (const Block& block : level.blocks) {
        if (overlaps(getBox(height), block.box)) {
            if (velocity.z > 0) {
                position.z = block.box.min.z - PLAYER_RADIUS;
                wallNormal = { 0, 0, -1 };
            } else {
                position.z = block.box.max.z + PLAYER_RADIUS;
                wallNormal = { 0, 0, 1 };
            }
            velocity.z = 0;
            touchingWall = true;
        }
    }

    // Y axis (floors and ceilings)
    position.y += velocity.y * dt;
    for (const Block& block : level.blocks) {
        if (overlaps(getBox(height), block.box)) {
            if (velocity.y <= 0) {
                position.y = block.box.max.y;   // landed on top
                grounded = true;
            } else {
                position.y = block.box.min.y - height;   // bonked our head
            }
            velocity.y = 0;
        }
    }

    // Fell out of the world? Back to spawn.
    if (position.y < -20) {
        spawn({ 0, 1, 0 }, 0);
    }
}

BoundingBox Player::getBox(float boxHeight) const {
    return {
        { position.x - PLAYER_RADIUS, position.y,             position.z - PLAYER_RADIUS },
        { position.x + PLAYER_RADIUS, position.y + boxHeight, position.z + PLAYER_RADIUS }
    };
}

bool Player::canStandUp(const Level& level) const {
    for (const Block& block : level.blocks) {
        if (overlaps(getBox(STAND_HEIGHT), block.box)) {
            return false;
        }
    }
    return true;
}

void Player::addRecoil(float amount) {
    pitch = Clamp(pitch + amount, -1.5f, 1.5f);
}

Vector3 Player::getEyePosition() const {
    return { position.x, position.y + height - EYE_BELOW_TOP, position.z };
}

Vector3 Player::getLookDirection() const {
    return { sinf(yaw) * cosf(pitch), sinf(pitch), cosf(yaw) * cosf(pitch) };
}

Vector3 Player::getFlatForward() const {
    return { sinf(yaw), 0, cosf(yaw) };
}

Vector3 Player::getRight() const {
    return { -cosf(yaw), 0, sinf(yaw) };
}

float Player::getHorizontalSpeed() const {
    return sqrtf(velocity.x * velocity.x + velocity.z * velocity.z);
}
