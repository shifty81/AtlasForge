#include "../engine/animation/AnimationGraph.h"
#include <cassert>
#include <iostream>

using namespace atlas::animation;

void test_anim_default_state() {
    AnimationController ac;
    ac.Init();
    assert(ac.GetState() == AnimState::Idle);
    assert(ac.GetBlendWeight() == 1.0f);
    assert(ac.ModifierCount() == 0);
    std::cout << "[PASS] test_anim_default_state" << std::endl;
}

void test_anim_set_state() {
    AnimationController ac;
    ac.Init();
    ac.SetState(AnimState::Run);
    assert(ac.GetState() == AnimState::Run);
    ac.SetState(AnimState::Attack);
    assert(ac.GetState() == AnimState::Attack);
    std::cout << "[PASS] test_anim_set_state" << std::endl;
}

void test_anim_modifiers() {
    AnimationController ac;
    ac.Init();
    ac.AddModifier({AnimModifierSource::Damage, "upper_body", 0.5f});
    ac.AddModifier({AnimModifierSource::Skill, "arms", 0.8f});
    assert(ac.ModifierCount() == 2);
    ac.ClearModifiers();
    assert(ac.ModifierCount() == 0);
    std::cout << "[PASS] test_anim_modifiers" << std::endl;
}

void test_anim_effective_weight() {
    AnimationController ac;
    ac.Init();
    ac.AddModifier({AnimModifierSource::Damage, "upper_body", 0.5f});
    ac.AddModifier({AnimModifierSource::Damage, "lower_body", 0.3f});
    ac.AddModifier({AnimModifierSource::Skill, "arms", 1.0f});
    float dmgWeight = ac.ComputeEffectiveWeight(AnimModifierSource::Damage);
    assert(dmgWeight > 0.79f && dmgWeight < 0.81f);
    float skillWeight = ac.ComputeEffectiveWeight(AnimModifierSource::Skill);
    assert(skillWeight == 1.0f);
    float envWeight = ac.ComputeEffectiveWeight(AnimModifierSource::Environment);
    assert(envWeight == 0.0f);
    std::cout << "[PASS] test_anim_effective_weight" << std::endl;
}
