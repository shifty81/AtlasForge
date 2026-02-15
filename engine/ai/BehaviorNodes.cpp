#include "BehaviorNodes.h"
#include <algorithm>

namespace atlas::ai {

// --- ThreatAssessmentNode ---

std::vector<BehaviorPort> ThreatAssessmentNode::Outputs() const {
    return {{"Threat", BehaviorPinType::Perception}};
}

void ThreatAssessmentNode::Evaluate(const AIContext& ctx, const std::vector<BehaviorValue>& /*inputs*/, std::vector<BehaviorValue>& outputs) const {
    outputs.resize(1);
    outputs[0].type = BehaviorPinType::Perception;
    outputs[0].data = {ctx.threatLevel};
}

// --- UtilityScoreNode ---

std::vector<BehaviorPort> UtilityScoreNode::Inputs() const {
    return {
        {"ThreatScore", BehaviorPinType::Float},
        {"HealthScore", BehaviorPinType::Float}
    };
}

std::vector<BehaviorPort> UtilityScoreNode::Outputs() const {
    return {{"Score", BehaviorPinType::Float}};
}

void UtilityScoreNode::Evaluate(const AIContext& /*ctx*/, const std::vector<BehaviorValue>& inputs, std::vector<BehaviorValue>& outputs) const {
    float score = 0.0f;

    if (!inputs.empty() && !inputs[0].data.empty()) {
        score += inputs[0].data[0] * threatWeight;
    }
    if (inputs.size() > 1 && !inputs[1].data.empty()) {
        score += inputs[1].data[0] * healthWeight;
    }

    outputs.resize(1);
    outputs[0].type = BehaviorPinType::Float;
    outputs[0].data = {score};
}

// --- ActionSelectorNode ---

std::vector<BehaviorPort> ActionSelectorNode::Inputs() const {
    return {
        {"UtilityA", BehaviorPinType::Float},
        {"UtilityB", BehaviorPinType::Float}
    };
}

std::vector<BehaviorPort> ActionSelectorNode::Outputs() const {
    return {{"SelectedAction", BehaviorPinType::Action}};
}

void ActionSelectorNode::Evaluate(const AIContext& /*ctx*/, const std::vector<BehaviorValue>& inputs, std::vector<BehaviorValue>& outputs) const {
    float scoreA = 0.0f;
    float scoreB = 0.0f;

    if (!inputs.empty() && !inputs[0].data.empty()) {
        scoreA = inputs[0].data[0];
    }
    if (inputs.size() > 1 && !inputs[1].data.empty()) {
        scoreB = inputs[1].data[0];
    }

    // Output: index of winning action (0 or 1) and its score
    float selected = (scoreA >= scoreB) ? 0.0f : 1.0f;

    outputs.resize(1);
    outputs[0].type = BehaviorPinType::Action;
    outputs[0].data = {selected, std::max(scoreA, scoreB)};
}

// --- EmotionUpdateNode ---

std::vector<BehaviorPort> EmotionUpdateNode::Outputs() const {
    return {{"Emotions", BehaviorPinType::EmotionState}};
}

void EmotionUpdateNode::Evaluate(const AIContext& ctx, const std::vector<BehaviorValue>& /*inputs*/, std::vector<BehaviorValue>& outputs) const {
    // Emotion vector: [fear, confidence, anger]
    float fear = ctx.threatLevel * (1.0f - ctx.morale);
    float confidence = ctx.morale * ctx.healthPercent;
    float anger = ctx.threatLevel * ctx.morale;

    outputs.resize(1);
    outputs[0].type = BehaviorPinType::EmotionState;
    outputs[0].data = {fear, confidence, anger};
}

}
