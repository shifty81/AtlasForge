#include "../engine/tile/TileGraph.h"
#include <cassert>
#include <iostream>

using namespace atlas::tile;

void test_tile_init() {
    TileMap tm;
    tm.Init(10, 8);
    assert(tm.Width() == 10);
    assert(tm.Height() == 8);
    assert(tm.DefinitionCount() == 0);
    std::cout << "[PASS] test_tile_init" << std::endl;
}

void test_tile_register() {
    TileMap tm;
    tm.Init(4, 4);
    uint32_t id1 = tm.RegisterTile("Grass", TileType::Ground, {true, true, false});
    uint32_t id2 = tm.RegisterTile("Stone", TileType::Wall, {false, false, true});
    assert(tm.DefinitionCount() == 2);
    const TileDefinition* def = tm.GetDefinition(id1);
    assert(def != nullptr);
    assert(def->name == "Grass");
    assert(def->flags.walkable == true);
    const TileDefinition* def2 = tm.GetDefinition(id2);
    assert(def2 != nullptr);
    assert(def2->flags.destructible == true);
    std::cout << "[PASS] test_tile_register" << std::endl;
}

void test_tile_set_get() {
    TileMap tm;
    tm.Init(4, 4);
    uint32_t grassId = tm.RegisterTile("Grass", TileType::Ground, {true, true, false});
    tm.SetTile(1, 2, grassId);
    const TileInstance* inst = tm.GetTile(1, 2);
    assert(inst != nullptr);
    assert(inst->defId == grassId);
    assert(inst->damage == 0.0f);
    std::cout << "[PASS] test_tile_set_get" << std::endl;
}

void test_tile_damage() {
    TileMap tm;
    tm.Init(4, 4);
    uint32_t wallId = tm.RegisterTile("Wall", TileType::Wall, {false, false, true});
    tm.SetTile(0, 0, wallId);
    tm.DamageTile(0, 0, 10.0f);
    tm.DamageTile(0, 0, 5.0f);
    const TileInstance* inst = tm.GetTile(0, 0);
    assert(inst != nullptr);
    assert(inst->damage == 15.0f);
    std::cout << "[PASS] test_tile_damage" << std::endl;
}

void test_tile_bounds() {
    TileMap tm;
    tm.Init(3, 3);
    assert(tm.IsValid(0, 0) == true);
    assert(tm.IsValid(2, 2) == true);
    assert(tm.IsValid(3, 0) == false);
    assert(tm.IsValid(0, 3) == false);
    assert(tm.GetTile(99, 99) == nullptr);
    std::cout << "[PASS] test_tile_bounds" << std::endl;
}
