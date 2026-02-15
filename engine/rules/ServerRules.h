#pragma once
#include <string>
#include <unordered_map>
#include <cstdint>

namespace atlas::rules {

struct RuleDescriptor {
    std::string name;
    float baseValue = 1.0f;
    float multiplier = 1.0f;
    float minMultiplier = 0.0f;
    float maxMultiplier = 10.0f;
    bool hotReloadable = false;
    bool affectsReplay = false;
};

class ServerRules {
public:
    static ServerRules& Get();

    void RegisterRule(const RuleDescriptor& desc);
    bool HasRule(const std::string& name) const;

    float GetValue(const std::string& name) const;
    float GetMultiplier(const std::string& name) const;

    bool SetMultiplier(const std::string& name, float multiplier);

    const RuleDescriptor* GetDescriptor(const std::string& name) const;
    size_t RuleCount() const;

    void Clear();

private:
    std::unordered_map<std::string, RuleDescriptor> m_rules;
};

}
