#pragma once

// All the numbers that control how the game feels, in one place.
// Tweak these and rebuild to change the feel of the game.

// Window
const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;

// Camera
const float MOUSE_SENSITIVITY = 0.0025f;
const float HIP_FOV = 75.0f;
const float ADS_FOV = 55.0f;   // zoomed in when aiming down sights

// Player size (meters)
const float PLAYER_RADIUS = 0.35f;
const float STAND_HEIGHT = 1.8f;
const float CROUCH_HEIGHT = 1.1f;
const float EYE_BELOW_TOP = 0.15f;

// Movement speeds (meters per second)
const float WALK_SPEED = 5.0f;
const float SPRINT_SPEED = 8.0f;
const float CROUCH_SPEED = 2.5f;
const float ADS_SPEED = 3.0f;
const float GROUND_ACCEL = 60.0f;
const float AIR_ACCEL = 15.0f;

// Jumping and gravity
const float GRAVITY = 22.0f;
const float JUMP_SPEED = 7.5f;

// Sliding
const float SLIDE_BOOST = 11.0f;     // speed at the start of a slide
const float SLIDE_FRICTION = 7.0f;   // how fast the slide slows down
const float SLIDE_END_SPEED = 3.0f;  // slide stops below this speed

// Wall sliding
const float WALL_SLIDE_FALL_SPEED = 2.0f;  // max fall speed while on a wall
const float WALL_JUMP_UP = 7.0f;
const float WALL_JUMP_PUSH = 7.0f;         // push away from the wall

// Weapon (placeholder rifle)
const int MAG_SIZE = 30;
const int START_RESERVE_AMMO = 120;
const float FIRE_INTERVAL = 0.1f;     // seconds between shots (600 rounds/min)
const float RELOAD_TIME = 1.8f;
const float WEAPON_DAMAGE = 25.0f;
const float WEAPON_RANGE = 200.0f;
const float RECOIL_KICK = 0.012f;     // how much each shot kicks the view up
const float HIP_SPREAD = 0.025f;      // bullet spread from the hip
const float ADS_SPREAD = 0.003f;      // bullet spread when aiming
const float IMPACT_LIFETIME = 4.0f;   // how long bullet marks stay
