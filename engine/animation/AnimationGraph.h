#pragma once

#include <string>
#include <vector>

namespace atlas::animation {

enum class AnimState {
    Idle,
    Walk,
    Run,
    Attack,
    Hit,
    Death,
    Custom
};

enum class AnimModifierSource {
    Damage,
    Skill,
    Emotion,
    Environment,
    Equipment
};

struct AnimModifier {
    AnimModifierSource source;
    std::string boneMask;
    float weight;
};

struct AnimationState {
    AnimState state = AnimState::Idle;
    float blendWeight = 1.0f;
    float timeScale = 1.0f;
};

class AnimationController {
public:
    void Init();
    void SetState(AnimState state);
    AnimState GetState() const;
    float GetBlendWeight() const;
    void AddModifier(const AnimModifier& modifier);
    void ClearModifiers();
    size_t ModifierCount() const;
    float ComputeEffectiveWeight(AnimModifierSource source) const;
    void Update(float dt);

private:
    AnimationState m_current;
    std::vector<AnimModifier> m_modifiers;
};

} // namespace atlas::animation
