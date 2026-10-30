#include "Level.h"
#include "Config.h"

const float TARGET_HEALTH = 100.0f;
const float TARGET_RESPAWN_TIME = 3.0f;

BoundingBox Target::getBox() const {
    return {
        { position.x - 0.4f, position.y,        position.z - 0.4f },
        { position.x + 0.4f, position.y + 1.8f, position.z + 0.4f }
    };
}

// Helper: make a block from its center and size, so the map is easy to read.
static Block makeBlock(Vector3 center, Vector3 size, Color color) {
    Vector3 half = { size.x / 2, size.y / 2, size.z / 2 };
    BoundingBox box = {
        { center.x - half.x, center.y - half.y, center.z - half.z },
        { center.x + half.x, center.y + half.y, center.z + half.z }
    };
    return { box, color };
}

void Level::build() {
    blocks.clear();
    targets.clear();
    impacts.clear();

    // Floor (its top is at y = 0)
    blocks.push_back(makeBlock({ 0, -0.5f, 0 }, { 60, 1, 60 }, DARKGREEN));

    // Outer walls
    blocks.push_back(makeBlock({ 0, 3, 30 },  { 60, 6, 1 }, GRAY));
    blocks.push_back(makeBlock({ 0, 3, -30 }, { 60, 6, 1 }, GRAY));
    blocks.push_back(makeBlock({ 30, 3, 0 },  { 1, 6, 60 }, GRAY));
    blocks.push_back(makeBlock({ -30, 3, 0 }, { 1, 6, 60 }, GRAY));

    // Two tall parallel walls: jump between them to practice wall sliding
    blocks.push_back(makeBlock({ -12, 4, 5 }, { 1, 8, 14 }, LIGHTGRAY));
    blocks.push_back(makeBlock({ -16, 4, 5 }, { 1, 8, 14 }, LIGHTGRAY));

    // Crates to jump on and hide behind
    blocks.push_back(makeBlock({ 4, 0.5f, 6 },   { 1, 1, 1 }, BROWN));
    blocks.push_back(makeBlock({ 6, 0.75f, 8 },  { 1.5f, 1.5f, 1.5f }, BROWN));
    blocks.push_back(makeBlock({ 9, 1.0f, 10 },  { 2, 2, 2 }, BROWN));

    // A low overhang: slide under it
    blocks.push_back(makeBlock({ 0, 1.75f, -8 }, { 6, 0.5f, 3 }, MAROON));
    blocks.push_back(makeBlock({ -2.75f, 0.75f, -8 }, { 0.5f, 1.5f, 3 }, MAROON));
    blocks.push_back(makeBlock({ 2.75f, 0.75f, -8 },  { 0.5f, 1.5f, 3 }, MAROON));

    // Shooting range targets
    for (int i = 0; i < 5; i++) {
        targets.push_back({ { -8.0f + i * 4.0f, 0, 22 }, TARGET_HEALTH, 0 });
    }
}

void Level::update(float dt) {
    for (Target& target : targets) {
        if (!target.isAlive()) {
            target.respawnTimer -= dt;
            if (target.respawnTimer <= 0) {
                target.health = TARGET_HEALTH;
            }
        }
    }

    for (Impact& impact : impacts) {
        impact.timeLeft -= dt;
    }
    // Remove impacts whose time is up
    std::erase_if(impacts, [](const Impact& i) { return i.timeLeft <= 0; });
}

void Level::draw() const {
    for (const Block& block : blocks) {
        Vector3 size = {
            block.box.max.x - block.box.min.x,
            block.box.max.y - block.box.min.y,
            block.box.max.z - block.box.min.z
        };
        Vector3 center = {
            block.box.min.x + size.x / 2,
            block.box.min.y + size.y / 2,
            block.box.min.z + size.z / 2
        };
        DrawCubeV(center, size, block.color);
        DrawCubeWiresV(center, size, Fade(BLACK, 0.3f));
    }

    for (const Target& target : targets) {
        if (target.isAlive()) {
            Vector3 center = { target.position.x, target.position.y + 0.9f, target.position.z };
            Color color = ColorLerp(RED, ORANGE, 1.0f - target.health / TARGET_HEALTH);
            DrawCube(center, 0.8f, 1.8f, 0.8f, color);
            DrawCubeWires(center, 0.8f, 1.8f, 0.8f, BLACK);
        }
    }

    for (const Impact& impact : impacts) {
        DrawSphere(impact.position, 0.05f, Fade(BLACK, impact.timeLeft / IMPACT_LIFETIME));
    }
}
