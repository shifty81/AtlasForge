#include "AnimationGraph.h"

namespace atlas::animation {

void AnimationController::Init() {
    m_current = {};
    m_modifiers.clear();
}

void AnimationController::SetState(AnimState state) {
    m_current.state = state;
}

AnimState AnimationController::GetState() const {
    return m_current.state;
}

float AnimationController::GetBlendWeight() const {
    return m_current.blendWeight;
}

void AnimationController::AddModifier(const AnimModifier& modifier) {
    m_modifiers.push_back(modifier);
}

void AnimationController::ClearModifiers() {
    m_modifiers.clear();
}

size_t AnimationController::ModifierCount() const {
    return m_modifiers.size();
}

float AnimationController::ComputeEffectiveWeight(AnimModifierSource source) const {
    float total = 0.0f;
    for (const auto& mod : m_modifiers) {
        if (mod.source == source) {
            total += mod.weight;
        }
    }
    return total;
}

void AnimationController::Update(float dt) {
    m_current.timeScale = 1.0f;
    m_current.blendWeight = 1.0f;
    for (const auto& mod : m_modifiers) {
        m_current.blendWeight *= mod.weight;
    }
}

} // namespace atlas::animation
