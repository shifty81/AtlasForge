#pragma once
#include <cstdint>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>

namespace atlas::sound {

enum class GeneratorType : uint8_t {
    Sine,
    Saw,
    Square,
    Noise,
    Sample
};

enum class SoundNodeType : uint8_t {
    Generator,
    Filter,
    Envelope,
    Mixer,
    Output
};

struct SoundParam {
    std::string name;
    float value = 0.0f;
    float minVal = 0.0f;
    float maxVal = 1.0f;
};

struct SoundNode {
    uint32_t id = 0;
    SoundNodeType type = SoundNodeType::Generator;
    GeneratorType generatorType = GeneratorType::Sine;
    std::vector<SoundParam> params;
};

struct SoundActionBinding {
    std::string actionName;
    uint32_t soundNodeId = 0;
    std::map<std::string, std::string> paramBindings;
};

class SoundGraph {
public:
    void Init();
    uint32_t AddNode(SoundNodeType type, GeneratorType genType);
    void RemoveNode(uint32_t id);
    const SoundNode* GetNode(uint32_t id) const;
    size_t NodeCount() const;

    void SetParam(uint32_t nodeId, const std::string& paramName, float value);

    void BindAction(const std::string& actionName, uint32_t nodeId);
    const SoundActionBinding* GetBinding(const std::string& actionName) const;
    size_t BindingCount() const;

    float Evaluate(float time) const;

private:
    std::unordered_map<uint32_t, SoundNode> m_nodes;
    std::unordered_map<std::string, SoundActionBinding> m_bindings;
    uint32_t m_nextId = 1;
};

} // namespace atlas::sound
