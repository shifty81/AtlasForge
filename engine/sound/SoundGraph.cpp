#include "SoundGraph.h"
#include <cmath>

namespace atlas::sound {

void SoundGraph::Init() {
    m_nodes.clear();
    m_bindings.clear();
    m_nextId = 1;
}

uint32_t SoundGraph::AddNode(SoundNodeType type, GeneratorType genType) {
    uint32_t id = m_nextId++;
    SoundNode node;
    node.id = id;
    node.type = type;
    node.generatorType = genType;
    m_nodes[id] = node;
    return id;
}

void SoundGraph::RemoveNode(uint32_t id) {
    m_nodes.erase(id);
}

const SoundNode* SoundGraph::GetNode(uint32_t id) const {
    auto it = m_nodes.find(id);
    if (it != m_nodes.end()) {
        return &it->second;
    }
    return nullptr;
}

size_t SoundGraph::NodeCount() const {
    return m_nodes.size();
}

void SoundGraph::SetParam(uint32_t nodeId, const std::string& paramName, float value) {
    auto it = m_nodes.find(nodeId);
    if (it == m_nodes.end()) return;

    for (auto& p : it->second.params) {
        if (p.name == paramName) {
            p.value = value;
            return;
        }
    }
    SoundParam param;
    param.name = paramName;
    param.value = value;
    it->second.params.push_back(param);
}

void SoundGraph::BindAction(const std::string& actionName, uint32_t nodeId) {
    SoundActionBinding binding;
    binding.actionName = actionName;
    binding.soundNodeId = nodeId;
    m_bindings[actionName] = binding;
}

const SoundActionBinding* SoundGraph::GetBinding(const std::string& actionName) const {
    auto it = m_bindings.find(actionName);
    if (it != m_bindings.end()) {
        return &it->second;
    }
    return nullptr;
}

size_t SoundGraph::BindingCount() const {
    return m_bindings.size();
}

float SoundGraph::Evaluate(float time) const {
    float output = 0.0f;
    for (const auto& [id, node] : m_nodes) {
        if (node.type == SoundNodeType::Generator) {
            float freq = 440.0f;
            for (const auto& p : node.params) {
                if (p.name == "frequency") freq = p.value;
            }
            switch (node.generatorType) {
                case GeneratorType::Sine:
                    output += std::sin(2.0f * 3.14159265f * freq * time);
                    break;
                case GeneratorType::Saw:
                    output += 2.0f * (time * freq - std::floor(time * freq + 0.5f));
                    break;
                case GeneratorType::Square:
                    output += std::sin(2.0f * 3.14159265f * freq * time) >= 0.0f ? 1.0f : -1.0f;
                    break;
                case GeneratorType::Noise:
                    output += static_cast<float>(static_cast<int>(time * 1000.0f) % 200 - 100) / 100.0f;
                    break;
                case GeneratorType::Sample:
                    output += 0.0f;
                    break;
            }
        }
    }
    return output;
}

} // namespace atlas::sound
