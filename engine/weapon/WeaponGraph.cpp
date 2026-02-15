#include "WeaponGraph.h"

namespace atlas::weapon {

void WeaponInstance::Init() {
    m_modules.clear();
    m_wear = 0.0f;
}

void WeaponInstance::AddModule(const WeaponModule& module) {
    m_modules.push_back(module);
}

bool WeaponInstance::RemoveModule(uint32_t id) {
    for (auto it = m_modules.begin(); it != m_modules.end(); ++it) {
        if (it->id == id) {
            m_modules.erase(it);
            return true;
        }
    }
    return false;
}

const WeaponModule* WeaponInstance::GetModule(uint32_t id) const {
    for (const auto& mod : m_modules) {
        if (mod.id == id) {
            return &mod;
        }
    }
    return nullptr;
}

size_t WeaponInstance::ModuleCount() const {
    return m_modules.size();
}

WeaponStats WeaponInstance::ComputeStats() const {
    WeaponStats stats;
    for (const auto& mod : m_modules) {
        switch (mod.type) {
            case WeaponModuleType::Receiver:
                stats.damage += mod.statModifier;
                break;
            case WeaponModuleType::Barrel:
                stats.accuracy += mod.statModifier;
                break;
            case WeaponModuleType::Magazine:
                stats.fireRate += mod.statModifier;
                break;
            case WeaponModuleType::Optics:
                stats.accuracy += mod.statModifier;
                break;
            case WeaponModuleType::Stabilizer:
                stats.recoil += mod.statModifier;
                break;
        }
    }
    stats.wear = m_wear;
    return stats;
}

void WeaponInstance::ApplyWear(float amount) {
    m_wear += amount;
}

float WeaponInstance::GetWear() const {
    return m_wear;
}

WeaponStats WeaponInstance::GetStats() const {
    return ComputeStats();
}

} // namespace atlas::weapon
