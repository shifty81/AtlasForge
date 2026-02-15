#pragma once

#include <cstdint>
#include <map>
#include <string>
#include <vector>

namespace atlas::character {

enum class BodyType {
    Biped,
    Heavy,
    Digitigrade,
    Quadruped
};

enum class EquipmentSlot {
    Head,
    Torso,
    Legs,
    Feet,
    Hand_L,
    Hand_R,
    Back
};

struct CharacterTrait {
    std::string name;
    float value;
};

struct Equipment {
    EquipmentSlot slot;
    std::string name;
    float armorValue;
};

class CharacterProfile {
public:
    void Init();
    void SetBody(BodyType body);
    BodyType GetBody() const;
    void AddTrait(const std::string& name, float value);
    const CharacterTrait* GetTrait(const std::string& name) const;
    size_t TraitCount() const;
    void Equip(EquipmentSlot slot, const std::string& name, float armor);
    bool Unequip(EquipmentSlot slot);
    const Equipment* GetEquipment(EquipmentSlot slot) const;
    void SetFaction(uint32_t id);
    uint32_t GetFaction() const;

private:
    BodyType m_body = BodyType::Biped;
    std::vector<CharacterTrait> m_traits;
    std::map<EquipmentSlot, Equipment> m_equipment;
    uint32_t m_factionId = 0;
};

} // namespace atlas::character
