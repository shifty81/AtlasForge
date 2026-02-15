#include "../engine/weapon/WeaponGraph.h"
#include <cassert>
#include <iostream>

using namespace atlas::weapon;

void test_weapon_add_module() {
    WeaponInstance w;
    w.Init();
    w.AddModule({1, "Barrel_A", WeaponModuleType::Barrel, 5.0f});
    w.AddModule({2, "Receiver_A", WeaponModuleType::Receiver, 10.0f});
    assert(w.ModuleCount() == 2);
    assert(w.GetModule(1) != nullptr);
    assert(w.GetModule(1)->name == "Barrel_A");
    std::cout << "[PASS] test_weapon_add_module" << std::endl;
}

void test_weapon_compute_stats() {
    WeaponInstance w;
    w.Init();
    w.AddModule({1, "Receiver_A", WeaponModuleType::Receiver, 25.0f});
    w.AddModule({2, "Barrel_A", WeaponModuleType::Barrel, 10.0f});
    w.AddModule({3, "Stabilizer_A", WeaponModuleType::Stabilizer, -3.0f});
    WeaponStats stats = w.ComputeStats();
    assert(stats.damage == 25.0f);
    assert(stats.accuracy == 10.0f);
    assert(stats.recoil == -3.0f);
    std::cout << "[PASS] test_weapon_compute_stats" << std::endl;
}

void test_weapon_wear() {
    WeaponInstance w;
    w.Init();
    assert(w.GetWear() == 0.0f);
    w.ApplyWear(1.5f);
    w.ApplyWear(2.5f);
    assert(w.GetWear() == 4.0f);
    WeaponStats stats = w.GetStats();
    assert(stats.wear == 4.0f);
    std::cout << "[PASS] test_weapon_wear" << std::endl;
}

void test_weapon_remove_module() {
    WeaponInstance w;
    w.Init();
    w.AddModule({1, "Optics_A", WeaponModuleType::Optics, 8.0f});
    w.AddModule({2, "Magazine_A", WeaponModuleType::Magazine, 5.0f});
    assert(w.ModuleCount() == 2);
    assert(w.RemoveModule(1) == true);
    assert(w.ModuleCount() == 1);
    assert(w.GetModule(1) == nullptr);
    assert(w.RemoveModule(99) == false);
    std::cout << "[PASS] test_weapon_remove_module" << std::endl;
}
