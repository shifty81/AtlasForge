#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace atlas::weapon {

enum class WeaponModuleType {
    Receiver,
    Barrel,
    Magazine,
    Optics,
    Stabilizer
};

struct WeaponModule {
    uint32_t id;
    std::string name;
    WeaponModuleType type;
    float statModifier;
};

struct WeaponStats {
    float damage = 0.0f;
    float accuracy = 0.0f;
    float fireRate = 0.0f;
    float recoil = 0.0f;
    float heat = 0.0f;
    float wear = 0.0f;
};

class WeaponInstance {
public:
    void Init();
    void AddModule(const WeaponModule& module);
    bool RemoveModule(uint32_t id);
    const WeaponModule* GetModule(uint32_t id) const;
    size_t ModuleCount() const;
    WeaponStats ComputeStats() const;
    void ApplyWear(float amount);
    float GetWear() const;
    WeaponStats GetStats() const;

private:
    std::vector<WeaponModule> m_modules;
    float m_wear = 0.0f;
};

} // namespace atlas::weapon
