#include "WorldNodes.h"
#include "NoiseGenerator.h"
#include <algorithm>

namespace atlas::world {

static constexpr int kChunkRes = 64;
static constexpr int kFieldSize = kChunkRes * kChunkRes;

// --- SeedNode ---

std::vector<NodePort> SeedNode::Outputs() const {
    return {{"Seed", ValueType::Seed}};
}

void SeedNode::Evaluate(const WorldGenContext& ctx, const std::vector<Value>& /*inputs*/, std::vector<Value>& outputs) const {
    outputs.resize(1);
    outputs[0].type = ValueType::Seed;
    outputs[0].data = {static_cast<float>(ctx.worldSeed)};
}

// --- NoiseNode ---

std::vector<NodePort> NoiseNode::Inputs() const {
    return {
        {"Seed", ValueType::Seed},
        {"Frequency", ValueType::Float}
    };
}

std::vector<NodePort> NoiseNode::Outputs() const {
    return {{"Height", ValueType::HeightField}};
}

void NoiseNode::Evaluate(const WorldGenContext& ctx, const std::vector<Value>& inputs, std::vector<Value>& outputs) const {
    uint32_t seed = 0;
    if (!inputs.empty() && !inputs[0].data.empty()) {
        seed = static_cast<uint32_t>(inputs[0].data[0]);
    }

    float frequency = 0.01f;
    if (inputs.size() > 1 && !inputs[1].data.empty()) {
        frequency = inputs[1].data[0];
    }

    outputs.resize(1);
    outputs[0].type = ValueType::HeightField;
    outputs[0].data.resize(kFieldSize);

    float offsetX = static_cast<float>(ctx.chunkX * kChunkRes);
    float offsetZ = static_cast<float>(ctx.chunkZ * kChunkRes);

    for (int z = 0; z < kChunkRes; ++z) {
        for (int x = 0; x < kChunkRes; ++x) {
            float wx = (offsetX + static_cast<float>(x)) * frequency;
            float wz = (offsetZ + static_cast<float>(z)) * frequency;
            outputs[0].data[static_cast<size_t>(z) * kChunkRes + x] =
                NoiseGenerator::FBM2D(wx, wz, 6, 2.0f, 0.5f, seed);
        }
    }
}

// --- BlendNode ---

std::vector<NodePort> BlendNode::Inputs() const {
    return {
        {"A", ValueType::HeightField},
        {"B", ValueType::HeightField},
        {"Factor", ValueType::Float}
    };
}

std::vector<NodePort> BlendNode::Outputs() const {
    return {{"Out", ValueType::HeightField}};
}

void BlendNode::Evaluate(const WorldGenContext& /*ctx*/, const std::vector<Value>& inputs, std::vector<Value>& outputs) const {
    outputs.resize(1);
    outputs[0].type = ValueType::HeightField;
    outputs[0].data.resize(kFieldSize, 0.0f);

    float factor = 0.5f;
    if (inputs.size() > 2 && !inputs[2].data.empty()) {
        factor = inputs[2].data[0];
    }

    const auto& a = (inputs.size() > 0 && inputs[0].data.size() == kFieldSize)
        ? inputs[0].data : outputs[0].data;
    const auto& b = (inputs.size() > 1 && inputs[1].data.size() == kFieldSize)
        ? inputs[1].data : outputs[0].data;

    for (int i = 0; i < kFieldSize; ++i) {
        outputs[0].data[i] = a[i] + (b[i] - a[i]) * factor;
    }
}

// --- ClampNode ---

std::vector<NodePort> ClampNode::Inputs() const {
    return {
        {"In", ValueType::HeightField},
        {"Min", ValueType::Float},
        {"Max", ValueType::Float}
    };
}

std::vector<NodePort> ClampNode::Outputs() const {
    return {{"Out", ValueType::HeightField}};
}

void ClampNode::Evaluate(const WorldGenContext& /*ctx*/, const std::vector<Value>& inputs, std::vector<Value>& outputs) const {
    outputs.resize(1);
    outputs[0].type = ValueType::HeightField;

    float minVal = 0.0f;
    float maxVal = 1.0f;
    if (inputs.size() > 1 && !inputs[1].data.empty()) minVal = inputs[1].data[0];
    if (inputs.size() > 2 && !inputs[2].data.empty()) maxVal = inputs[2].data[0];

    if (!inputs.empty() && inputs[0].data.size() == kFieldSize) {
        outputs[0].data.resize(kFieldSize);
        for (int i = 0; i < kFieldSize; ++i) {
            outputs[0].data[i] = std::clamp(inputs[0].data[i], minVal, maxVal);
        }
    } else {
        outputs[0].data.resize(kFieldSize, std::clamp(0.0f, minVal, maxVal));
    }
}

// --- ConstantNode ---

std::vector<NodePort> ConstantNode::Outputs() const {
    return {{"Value", ValueType::Float}};
}

void ConstantNode::Evaluate(const WorldGenContext& /*ctx*/, const std::vector<Value>& /*inputs*/, std::vector<Value>& outputs) const {
    outputs.resize(1);
    outputs[0].type = ValueType::Float;
    outputs[0].data = {value};
}

}
