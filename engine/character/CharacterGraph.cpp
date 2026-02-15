#include "CharacterGraph.h"

namespace atlas::character {

void CharacterProfile::Init() {
    m_body = BodyType::Biped;
    m_traits.clear();
    m_equipment.clear();
    m_factionId = 0;
}

void CharacterProfile::SetBody(BodyType body) {
    m_body = body;
}

BodyType CharacterProfile::GetBody() const {
    return m_body;
}

void CharacterProfile::AddTrait(const std::string& name, float value) {
    m_traits.push_back({name, value});
}

const CharacterTrait* CharacterProfile::GetTrait(const std::string& name) const {
    for (const auto& trait : m_traits) {
        if (trait.name == name) {
            return &trait;
        }
    }
    return nullptr;
}

size_t CharacterProfile::TraitCount() const {
    return m_traits.size();
}

void CharacterProfile::Equip(EquipmentSlot slot, const std::string& name, float armor) {
    m_equipment[slot] = {slot, name, armor};
}

bool CharacterProfile::Unequip(EquipmentSlot slot) {
    return m_equipment.erase(slot) > 0;
}

const Equipment* CharacterProfile::GetEquipment(EquipmentSlot slot) const {
    auto it = m_equipment.find(slot);
    if (it != m_equipment.end()) {
        return &it->second;
    }
    return nullptr;
}

void CharacterProfile::SetFaction(uint32_t id) {
    m_factionId = id;
}

uint32_t CharacterProfile::GetFaction() const {
    return m_factionId;
}

} // namespace atlas::character
