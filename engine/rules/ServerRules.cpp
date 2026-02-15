#include "ServerRules.h"

namespace atlas::rules {

ServerRules& ServerRules::Get() {
    static ServerRules instance;
    return instance;
}

void ServerRules::RegisterRule(const RuleDescriptor& desc) {
    m_rules[desc.name] = desc;
}

bool ServerRules::HasRule(const std::string& name) const {
    return m_rules.count(name) > 0;
}

float ServerRules::GetValue(const std::string& name) const {
    auto it = m_rules.find(name);
    if (it == m_rules.end()) return 0.0f;
    return it->second.baseValue * it->second.multiplier;
}

float ServerRules::GetMultiplier(const std::string& name) const {
    auto it = m_rules.find(name);
    if (it == m_rules.end()) return 1.0f;
    return it->second.multiplier;
}

bool ServerRules::SetMultiplier(const std::string& name, float multiplier) {
    auto it = m_rules.find(name);
    if (it == m_rules.end()) return false;

    auto& rule = it->second;

    // Clamp to allowed range
    if (multiplier < rule.minMultiplier) multiplier = rule.minMultiplier;
    if (multiplier > rule.maxMultiplier) multiplier = rule.maxMultiplier;

    rule.multiplier = multiplier;
    return true;
}

const RuleDescriptor* ServerRules::GetDescriptor(const std::string& name) const {
    auto it = m_rules.find(name);
    if (it != m_rules.end()) return &it->second;
    return nullptr;
}

size_t ServerRules::RuleCount() const {
    return m_rules.size();
}

void ServerRules::Clear() {
    m_rules.clear();
}

}
