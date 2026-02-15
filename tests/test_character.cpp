#include "../engine/character/CharacterGraph.h"
#include <cassert>
#include <iostream>

using namespace atlas::character;

void test_character_body() {
    CharacterProfile cp;
    cp.Init();
    assert(cp.GetBody() == BodyType::Biped);
    cp.SetBody(BodyType::Quadruped);
    assert(cp.GetBody() == BodyType::Quadruped);
    std::cout << "[PASS] test_character_body" << std::endl;
}

void test_character_traits() {
    CharacterProfile cp;
    cp.Init();
    cp.AddTrait("Strength", 10.0f);
    cp.AddTrait("Agility", 8.0f);
    assert(cp.TraitCount() == 2);
    const CharacterTrait* t = cp.GetTrait("Strength");
    assert(t != nullptr);
    assert(t->value == 10.0f);
    assert(cp.GetTrait("Missing") == nullptr);
    std::cout << "[PASS] test_character_traits" << std::endl;
}

void test_character_equipment() {
    CharacterProfile cp;
    cp.Init();
    cp.Equip(EquipmentSlot::Head, "Helmet", 15.0f);
    cp.Equip(EquipmentSlot::Torso, "Chestplate", 30.0f);
    const Equipment* e = cp.GetEquipment(EquipmentSlot::Head);
    assert(e != nullptr);
    assert(e->name == "Helmet");
    assert(e->armorValue == 15.0f);
    assert(cp.Unequip(EquipmentSlot::Head) == true);
    assert(cp.GetEquipment(EquipmentSlot::Head) == nullptr);
    assert(cp.Unequip(EquipmentSlot::Head) == false);
    std::cout << "[PASS] test_character_equipment" << std::endl;
}

void test_character_faction() {
    CharacterProfile cp;
    cp.Init();
    assert(cp.GetFaction() == 0);
    cp.SetFaction(42);
    assert(cp.GetFaction() == 42);
    std::cout << "[PASS] test_character_faction" << std::endl;
}
