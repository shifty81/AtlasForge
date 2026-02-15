#include "CharacterNodes.h"
#include <cmath>

namespace atlas::character {

// Deterministic hash for character generation
static float CharHash(uint64_t seed, uint32_t index) {
    uint64_t h = seed;
    h ^= static_cast<uint64_t>(index) * 2654435761ULL;
    h = (h ^ (h >> 17)) * 1274126177ULL;
    h = h ^ (h >> 16);
    return static_cast<float>(h & 0xFFFFu) / 65535.0f;
}

// --- BaseBodyNode ---

std::vector<CharacterPort> BaseBodyNode::Inputs() const {
    return {{"Seed", CharacterPinType::Seed}};
}

std::vector<CharacterPort> BaseBodyNode::Outputs() const {
    return {{"Body", CharacterPinType::Mesh}};
}

void BaseBodyNode::Evaluate(const CharacterContext& ctx, const std::vector<CharacterValue>& inputs, std::vector<CharacterValue>& outputs) const {
    uint64_t seed = ctx.seed;
    if (!inputs.empty() && !inputs[0].data.empty()) {
        seed = static_cast<uint64_t>(inputs[0].data[0]);
    }

    // Generate height (1.5m - 2.1m), mass (50 - 120), proportions (limb ratios)
    float height = 1.5f + CharHash(seed, 0) * 0.6f;
    float mass = 50.0f + CharHash(seed, 1) * 70.0f;
    float armRatio = 0.8f + CharHash(seed, 2) * 0.4f;
    float legRatio = 0.9f + CharHash(seed, 3) * 0.2f;

    // Apply fitness modifier
    mass *= (1.0f - ctx.fitness * 0.2f);

    outputs.resize(1);
    outputs[0].type = CharacterPinType::Mesh;
    outputs[0].data = {height, mass, armRatio, legRatio};
}

// --- SkeletonNode ---

std::vector<CharacterPort> SkeletonNode::Inputs() const {
    return {{"Height", CharacterPinType::Float}};
}

std::vector<CharacterPort> SkeletonNode::Outputs() const {
    return {{"Skeleton", CharacterPinType::Skeleton}};
}

void SkeletonNode::Evaluate(const CharacterContext& /*ctx*/, const std::vector<CharacterValue>& inputs, std::vector<CharacterValue>& outputs) const {
    float height = 1.8f;
    if (!inputs.empty() && !inputs[0].data.empty()) {
        height = inputs[0].data[0];
    }

    // Generate bone lengths scaled by height
    float spine = height * 0.35f;
    float upperArm = height * 0.18f;
    float forearm = height * 0.15f;
    float thigh = height * 0.25f;
    float shin = height * 0.22f;

    outputs.resize(1);
    outputs[0].type = CharacterPinType::Skeleton;
    outputs[0].data = {spine, upperArm, forearm, thigh, shin};
}

// --- MaterialNode ---

std::vector<CharacterPort> MaterialNode::Inputs() const {
    return {
        {"Seed", CharacterPinType::Seed},
        {"FactionWeight", CharacterPinType::Float}
    };
}

std::vector<CharacterPort> MaterialNode::Outputs() const {
    return {{"Appearance", CharacterPinType::Material}};
}

void MaterialNode::Evaluate(const CharacterContext& ctx, const std::vector<CharacterValue>& inputs, std::vector<CharacterValue>& outputs) const {
    uint64_t seed = ctx.seed;
    if (!inputs.empty() && !inputs[0].data.empty()) {
        seed = static_cast<uint64_t>(inputs[0].data[0]);
    }

    float factionWeight = 1.0f;
    if (inputs.size() > 1 && !inputs[1].data.empty()) {
        factionWeight = inputs[1].data[0];
    }

    // Generate skin tone, hair color, eye color (as normalized floats)
    float skinTone = CharHash(seed, 10);
    float hairColor = CharHash(seed, 11);
    float eyeColor = CharHash(seed, 12);

    // Faction tints appearance
    float factionHue = static_cast<float>(ctx.factionID % 8) / 8.0f;
    hairColor = hairColor * (1.0f - factionWeight * 0.3f) + factionHue * factionWeight * 0.3f;

    outputs.resize(1);
    outputs[0].type = CharacterPinType::Material;
    outputs[0].data = {skinTone, hairColor, eyeColor};
}

// --- EquipmentNode ---

std::vector<CharacterPort> EquipmentNode::Inputs() const {
    return {
        {"Body", CharacterPinType::Mesh},
        {"Gear", CharacterPinType::Equipment}
    };
}

std::vector<CharacterPort> EquipmentNode::Outputs() const {
    return {{"Equipped", CharacterPinType::Mesh}};
}

void EquipmentNode::Evaluate(const CharacterContext& /*ctx*/, const std::vector<CharacterValue>& inputs, std::vector<CharacterValue>& outputs) const {
    outputs.resize(1);
    outputs[0].type = CharacterPinType::Mesh;

    // Start with body mesh data
    if (!inputs.empty() && !inputs[0].data.empty()) {
        outputs[0].data = inputs[0].data;
    }

    // Append equipment attachment data
    if (inputs.size() > 1 && !inputs[1].data.empty()) {
        outputs[0].data.insert(outputs[0].data.end(),
                               inputs[1].data.begin(),
                               inputs[1].data.end());
    }
}

}
