#include "../engine/procedural/InteriorNode.h"
#include "../engine/procedural/HullMeshGenerator.h"
#include "../engine/procedural/ModuleTier.h"
#include "../engine/procedural/BuildQueue.h"
#include "../engine/procedural/PlanetaryBase.h"
#include "../engine/procedural/ModuleScaling.h"
#include <cassert>
#include <iostream>
#include <cmath>
#include <vector>

using namespace atlas::procedural;

// ============================================================
// InteriorNode tests
// ============================================================

void test_interior_add_slot() {
    InteriorNode node;
    assert(node.SlotCount() == 0);
    ModuleSlot s1;
    s1.x = 1.0f; s1.y = 2.0f; s1.z = 3.0f;
    s1.size = ModuleSize::M;
    s1.allowedTypes = { ModuleType::Deck, ModuleType::Storage };
    size_t idx0 = node.AddSlot(s1);
    assert(idx0 == 0);
    assert(node.SlotCount() == 1);

    ModuleSlot s2;
    s2.size = ModuleSize::L;
    s2.allowedTypes = { ModuleType::PowerPlant };
    size_t idx1 = node.AddSlot(s2);
    assert(idx1 == 1);
    assert(node.SlotCount() == 2);
    std::cout << "[PASS] test_interior_add_slot" << std::endl;
}

void test_interior_place_module() {
    InteriorNode node;
    ModuleSlot slot;
    slot.size = ModuleSize::M;
    slot.allowedTypes = { ModuleType::Deck, ModuleType::Storage };
    node.AddSlot(slot);
    assert(node.OccupiedSlotCount() == 0);

    InteriorModule mod;
    mod.id = 1;
    mod.type = ModuleType::Deck;
    mod.size = ModuleSize::M;
    bool ok = node.PlaceModule(0, mod);
    assert(ok);
    assert(node.OccupiedSlotCount() == 1);
    assert(node.GetModule(0) != nullptr);
    assert(node.GetModule(0)->type == ModuleType::Deck);
    std::cout << "[PASS] test_interior_place_module" << std::endl;
}

void test_interior_place_invalid_type() {
    InteriorNode node;
    ModuleSlot slot;
    slot.size = ModuleSize::M;
    slot.allowedTypes = { ModuleType::Deck };
    node.AddSlot(slot);

    InteriorModule mod;
    mod.id = 1;
    mod.type = ModuleType::Lab;
    mod.size = ModuleSize::M;
    bool ok = node.PlaceModule(0, mod);
    assert(!ok);
    assert(node.OccupiedSlotCount() == 0);
    std::cout << "[PASS] test_interior_place_invalid_type" << std::endl;
}

void test_interior_place_invalid_size() {
    InteriorNode node;
    ModuleSlot slot;
    slot.size = ModuleSize::S;
    slot.allowedTypes = { ModuleType::Storage };
    node.AddSlot(slot);

    InteriorModule mod;
    mod.id = 1;
    mod.type = ModuleType::Storage;
    mod.size = ModuleSize::L;
    bool ok = node.PlaceModule(0, mod);
    assert(!ok);
    assert(node.OccupiedSlotCount() == 0);
    std::cout << "[PASS] test_interior_place_invalid_size" << std::endl;
}

void test_interior_remove_module() {
    InteriorNode node;
    ModuleSlot slot;
    slot.size = ModuleSize::M;
    slot.allowedTypes = { ModuleType::Deck };
    node.AddSlot(slot);

    InteriorModule mod;
    mod.id = 1;
    mod.type = ModuleType::Deck;
    mod.size = ModuleSize::M;
    node.PlaceModule(0, mod);
    assert(node.OccupiedSlotCount() == 1);

    node.RemoveModule(0);
    assert(node.OccupiedSlotCount() == 0);
    assert(node.GetModule(0) == nullptr);
    assert(node.SlotCount() == 1);
    std::cout << "[PASS] test_interior_remove_module" << std::endl;
}

void test_interior_find_slots_by_type() {
    InteriorNode node;
    ModuleSlot s1;
    s1.size = ModuleSize::S;
    s1.allowedTypes = { ModuleType::Deck, ModuleType::Storage };
    node.AddSlot(s1);

    ModuleSlot s2;
    s2.size = ModuleSize::M;
    s2.allowedTypes = { ModuleType::Lab };
    node.AddSlot(s2);

    ModuleSlot s3;
    s3.size = ModuleSize::L;
    s3.allowedTypes = { ModuleType::Storage, ModuleType::PowerPlant };
    node.AddSlot(s3);

    auto storageSlots = node.FindSlotsByType(ModuleType::Storage);
    assert(storageSlots.size() == 2);
    assert(storageSlots[0] == 0);
    assert(storageSlots[1] == 2);

    auto labSlots = node.FindSlotsByType(ModuleType::Lab);
    assert(labSlots.size() == 1);
    assert(labSlots[0] == 1);

    auto bridgeSlots = node.FindSlotsByType(ModuleType::Bridge);
    assert(bridgeSlots.empty());
    std::cout << "[PASS] test_interior_find_slots_by_type" << std::endl;
}

void test_interior_find_slots_by_size() {
    InteriorNode node;
    ModuleSlot s1; s1.size = ModuleSize::S; s1.allowedTypes = { ModuleType::Deck };
    ModuleSlot s2; s2.size = ModuleSize::M; s2.allowedTypes = { ModuleType::Deck };
    ModuleSlot s3; s3.size = ModuleSize::S; s3.allowedTypes = { ModuleType::Deck };
    node.AddSlot(s1);
    node.AddSlot(s2);
    node.AddSlot(s3);

    auto small = node.FindSlotsBySize(ModuleSize::S);
    assert(small.size() == 2);
    assert(small[0] == 0);
    assert(small[1] == 2);

    auto medium = node.FindSlotsBySize(ModuleSize::M);
    assert(medium.size() == 1);
    assert(medium[0] == 1);

    auto xl = node.FindSlotsBySize(ModuleSize::XL);
    assert(xl.empty());
    std::cout << "[PASS] test_interior_find_slots_by_size" << std::endl;
}

void test_interior_get_modules_by_type() {
    InteriorNode node;
    ModuleSlot s1; s1.size = ModuleSize::M; s1.allowedTypes = { ModuleType::Deck };
    ModuleSlot s2; s2.size = ModuleSize::M; s2.allowedTypes = { ModuleType::Storage };
    ModuleSlot s3; s3.size = ModuleSize::M; s3.allowedTypes = { ModuleType::Deck };
    node.AddSlot(s1);
    node.AddSlot(s2);
    node.AddSlot(s3);

    InteriorModule d1; d1.id = 1; d1.type = ModuleType::Deck; d1.size = ModuleSize::M;
    InteriorModule st; st.id = 2; st.type = ModuleType::Storage; st.size = ModuleSize::M;
    InteriorModule d2; d2.id = 3; d2.type = ModuleType::Deck; d2.size = ModuleSize::M;
    node.PlaceModule(0, d1);
    node.PlaceModule(1, st);
    node.PlaceModule(2, d2);

    auto decks = node.GetModulesByType(ModuleType::Deck);
    assert(decks.size() == 2);
    assert(decks[0].id == 1);
    assert(decks[1].id == 3);

    auto stores = node.GetModulesByType(ModuleType::Storage);
    assert(stores.size() == 1);
    assert(stores[0].id == 2);

    auto labs = node.GetModulesByType(ModuleType::Lab);
    assert(labs.empty());
    std::cout << "[PASS] test_interior_get_modules_by_type" << std::endl;
}

void test_interior_has_power() {
    InteriorNode node;
    ModuleSlot slot;
    slot.size = ModuleSize::L;
    slot.allowedTypes = { ModuleType::PowerPlant };
    node.AddSlot(slot);

    assert(!node.HasPower());

    InteriorModule pp;
    pp.id = 1;
    pp.type = ModuleType::PowerPlant;
    pp.size = ModuleSize::L;
    pp.health = 100.0f;
    pp.powered = true;
    node.PlaceModule(0, pp);
    assert(node.HasPower());

    node.RemoveModule(0);
    InteriorModule dead;
    dead.id = 2;
    dead.type = ModuleType::PowerPlant;
    dead.size = ModuleSize::L;
    dead.health = 0.0f;
    dead.powered = true;
    node.PlaceModule(0, dead);
    assert(!node.HasPower());
    std::cout << "[PASS] test_interior_has_power" << std::endl;
}

void test_interior_clear() {
    InteriorNode node;
    ModuleSlot s1; s1.size = ModuleSize::S; s1.allowedTypes = { ModuleType::Deck };
    ModuleSlot s2; s2.size = ModuleSize::M; s2.allowedTypes = { ModuleType::Lab };
    node.AddSlot(s1);
    node.AddSlot(s2);

    InteriorModule mod;
    mod.id = 1; mod.type = ModuleType::Deck; mod.size = ModuleSize::S;
    node.PlaceModule(0, mod);
    assert(node.OccupiedSlotCount() == 1);

    node.Clear();
    assert(node.OccupiedSlotCount() == 0);
    assert(node.SlotCount() == 2);
    assert(node.GetModule(0) == nullptr);
    assert(node.GetModule(1) == nullptr);
    std::cout << "[PASS] test_interior_clear" << std::endl;
}

// ============================================================
// HullMeshGenerator tests
// ============================================================

void test_hull_convex_hull_tetrahedron() {
    HullMeshGenerator gen;
    std::vector<HullVertex> pts = {
        {0.0f, 0.0f, 0.0f},
        {1.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f},
        {0.0f, 0.0f, 1.0f}
    };
    HullMesh mesh = gen.GenerateConvexHull(pts);
    assert(mesh.IsValid());
    assert(mesh.VertexCount() == 4);
    assert(mesh.TriangleCount() == 4);
    std::cout << "[PASS] test_hull_convex_hull_tetrahedron" << std::endl;
}

void test_hull_convex_hull_cube() {
    HullMeshGenerator gen;
    std::vector<HullVertex> pts = {
        {0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f},
        {1.0f, 1.0f, 0.0f}, {0.0f, 1.0f, 0.0f},
        {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f, 1.0f},
        {1.0f, 1.0f, 1.0f}, {0.0f, 1.0f, 1.0f}
    };
    HullMesh mesh = gen.GenerateConvexHull(pts);
    assert(mesh.IsValid());
    assert(mesh.VertexCount() == 8);
    assert(mesh.TriangleCount() >= 6);
    std::cout << "[PASS] test_hull_convex_hull_cube" << std::endl;
}

void test_hull_fill_gaps() {
    HullMeshGenerator gen;
    HullGeneratorConfig cfg;
    cfg.gapFillThreshold = 0.5f;
    gen.SetConfig(cfg);

    HullMesh mesh;
    mesh.vertices = {
        {0.0f, 0.0f, 0.0f},
        {10.0f, 0.0f, 0.0f},
        {0.0f, 10.0f, 0.0f}
    };
    mesh.triangles = { {0, 1, 2} };
    size_t vertsBefore = mesh.VertexCount();
    gen.FillGaps(mesh);
    assert(mesh.VertexCount() > vertsBefore);
    std::cout << "[PASS] test_hull_fill_gaps" << std::endl;
}

void test_hull_subdivide() {
    HullMeshGenerator gen;
    std::vector<HullVertex> pts = {
        {0.0f, 0.0f, 0.0f},
        {1.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f},
        {0.0f, 0.0f, 1.0f}
    };
    HullMesh mesh = gen.GenerateConvexHull(pts);
    size_t triBefore = mesh.TriangleCount();
    gen.Subdivide(mesh);
    assert(mesh.TriangleCount() == triBefore * 4);
    std::cout << "[PASS] test_hull_subdivide" << std::endl;
}

void test_hull_smooth() {
    HullMeshGenerator gen;
    std::vector<HullVertex> pts = {
        {0.0f, 0.0f, 0.0f},
        {2.0f, 0.0f, 0.0f},
        {0.0f, 2.0f, 0.0f},
        {0.0f, 0.0f, 2.0f}
    };
    HullMesh mesh = gen.GenerateConvexHull(pts);
    size_t vertCount = mesh.VertexCount();
    float origX0 = mesh.vertices[0].x;
    float origY0 = mesh.vertices[0].y;
    float origZ0 = mesh.vertices[0].z;

    HullGeneratorConfig cfg;
    cfg.smoothIterations = 5;
    gen.SetConfig(cfg);
    gen.Smooth(mesh);

    assert(mesh.VertexCount() == vertCount);
    bool moved = (mesh.vertices[0].x != origX0 ||
                  mesh.vertices[0].y != origY0 ||
                  mesh.vertices[0].z != origZ0);
    assert(moved);
    std::cout << "[PASS] test_hull_smooth" << std::endl;
}

void test_hull_generate_from_interior() {
    HullMeshGenerator gen;
    HullGeneratorConfig cfg;
    cfg.gapFillThreshold = 2.0f;
    cfg.smoothIterations = 1;
    cfg.subdivisionLevel = 1;
    gen.SetConfig(cfg);

    std::vector<HullVertex> positions = {
        {0.0f, 0.0f, 0.0f},
        {5.0f, 0.0f, 0.0f},
        {0.0f, 5.0f, 0.0f},
        {0.0f, 0.0f, 5.0f},
        {2.5f, 2.5f, 2.5f}
    };
    HullMesh mesh = gen.GenerateFromInterior(positions);
    assert(mesh.IsValid());
    assert(mesh.VertexCount() > 0);
    assert(mesh.TriangleCount() > 0);
    const HullMesh& cached = gen.GetLastMesh();
    assert(cached.VertexCount() == mesh.VertexCount());
    std::cout << "[PASS] test_hull_generate_from_interior" << std::endl;
}

void test_hull_mesh_centroid() {
    HullMesh mesh;
    mesh.vertices = {
        {0.0f, 0.0f, 0.0f},
        {3.0f, 0.0f, 0.0f},
        {0.0f, 6.0f, 0.0f}
    };
    mesh.triangles = { {0, 1, 2} };
    HullVertex c = mesh.Centroid();
    assert(std::fabs(c.x - 1.0f) < 0.001f);
    assert(std::fabs(c.y - 2.0f) < 0.001f);
    assert(std::fabs(c.z - 0.0f) < 0.001f);
    std::cout << "[PASS] test_hull_mesh_centroid" << std::endl;
}

// ============================================================
// ModuleTier tests
// ============================================================

void test_tier_bonus_values() {
    ModuleTierSystem tiers;
    TierBonus b1 = tiers.GetBonus(TierLevel::Tier1);
    assert(std::fabs(b1.healthMultiplier - 1.0f) < 0.001f);

    TierBonus b5 = tiers.GetBonus(TierLevel::Tier5);
    assert(std::fabs(b5.healthMultiplier - 3.0f) < 0.001f);
    assert(b5.healthMultiplier > b1.healthMultiplier);
    std::cout << "[PASS] test_tier_bonus_values" << std::endl;
}

void test_tier_visual_progression() {
    ModuleTierSystem tiers;
    TierVisual v1 = tiers.GetVisual(TierLevel::Tier1);
    TierVisual v3 = tiers.GetVisual(TierLevel::Tier3);
    TierVisual v5 = tiers.GetVisual(TierLevel::Tier5);
    assert(v5.glowIntensity > v3.glowIntensity);
    assert(v3.glowIntensity > v1.glowIntensity);
    std::cout << "[PASS] test_tier_visual_progression" << std::endl;
}

void test_tier_upgrade_requirement() {
    ModuleTierSystem tiers;
    TierRequirement r12 = tiers.GetUpgradeRequirement(TierLevel::Tier1, TierLevel::Tier2);
    TierRequirement r34 = tiers.GetUpgradeRequirement(TierLevel::Tier3, TierLevel::Tier4);
    TierRequirement r45 = tiers.GetUpgradeRequirement(TierLevel::Tier4, TierLevel::Tier5);
    assert(r45.creditsCost > r34.creditsCost);
    assert(r34.creditsCost > r12.creditsCost);
    std::cout << "[PASS] test_tier_upgrade_requirement" << std::endl;
}

void test_tier_can_upgrade() {
    ModuleTierSystem tiers;
    TierRequirement req = tiers.GetUpgradeRequirement(TierLevel::Tier1, TierLevel::Tier2);
    assert(tiers.CanUpgrade(TierLevel::Tier1, TierLevel::Tier2,
                            req.creditsCost, req.requiredTechLevel));
    assert(!tiers.CanUpgrade(TierLevel::Tier1, TierLevel::Tier2,
                             0, req.requiredTechLevel));
    assert(!tiers.CanUpgrade(TierLevel::Tier1, TierLevel::Tier2,
                             req.creditsCost, 0));
    std::cout << "[PASS] test_tier_can_upgrade" << std::endl;
}

void test_tier_apply_bonus() {
    ModuleTierSystem tiers;
    float base = 100.0f;
    float t1 = tiers.ApplyBonus(base, TierLevel::Tier1);
    assert(std::fabs(t1 - 100.0f) < 0.001f);

    float t5 = tiers.ApplyBonus(base, TierLevel::Tier5);
    assert(std::fabs(t5 - 300.0f) < 0.001f);
    std::cout << "[PASS] test_tier_apply_bonus" << std::endl;
}

void test_tier_name() {
    assert(std::string(ModuleTierSystem::TierName(TierLevel::Tier1)) == "Tier 1");
    assert(std::string(ModuleTierSystem::TierName(TierLevel::Tier2)) == "Tier 2");
    assert(std::string(ModuleTierSystem::TierName(TierLevel::Tier3)) == "Tier 3");
    assert(std::string(ModuleTierSystem::TierName(TierLevel::Tier4)) == "Tier 4");
    assert(std::string(ModuleTierSystem::TierName(TierLevel::Tier5)) == "Tier 5");
    std::cout << "[PASS] test_tier_name" << std::endl;
}

// ============================================================
// BuildQueue tests
// ============================================================

void test_build_queue_add_order() {
    BuildQueue queue;
    assert(queue.IsEmpty());

    BuildOrder o1;
    o1.type = BuildOrderType::Construct;
    o1.totalTimeSeconds = 10.0f;
    uint32_t id1 = queue.AddOrder(o1);

    BuildOrder o2;
    o2.type = BuildOrderType::Upgrade;
    o2.totalTimeSeconds = 20.0f;
    uint32_t id2 = queue.AddOrder(o2);

    assert(id1 != id2);
    assert(queue.QueueSize() == 2);
    assert(!queue.IsEmpty());
    std::cout << "[PASS] test_build_queue_add_order" << std::endl;
}

void test_build_queue_tick() {
    BuildQueue queue;
    BuildOrder o;
    o.type = BuildOrderType::Construct;
    o.totalTimeSeconds = 5.0f;
    uint32_t id = queue.AddOrder(o);

    queue.Tick(3.0f);
    const BuildOrder* active = queue.GetOrder(id);
    assert(active != nullptr);
    assert(std::fabs(active->elapsedSeconds - 3.0f) < 0.001f);
    assert(!active->IsComplete());

    queue.Tick(3.0f);
    assert(queue.CompletedCount() == 1);
    assert(queue.QueueSize() == 0);
    std::cout << "[PASS] test_build_queue_tick" << std::endl;
}

void test_build_queue_pause_resume() {
    BuildQueue queue;
    BuildOrder o;
    o.type = BuildOrderType::Repair;
    o.totalTimeSeconds = 10.0f;
    uint32_t id = queue.AddOrder(o);

    queue.Tick(2.0f);
    queue.PauseOrder(id);
    queue.Tick(5.0f);
    const BuildOrder* paused = queue.GetOrder(id);
    assert(paused != nullptr);
    assert(std::fabs(paused->elapsedSeconds - 2.0f) < 0.001f);

    queue.ResumeOrder(id);
    queue.Tick(1.0f);
    const BuildOrder* resumed = queue.GetOrder(id);
    assert(resumed != nullptr);
    assert(std::fabs(resumed->elapsedSeconds - 3.0f) < 0.001f);
    std::cout << "[PASS] test_build_queue_pause_resume" << std::endl;
}

void test_build_queue_remove() {
    BuildQueue queue;
    BuildOrder o;
    o.type = BuildOrderType::Dismantle;
    o.totalTimeSeconds = 10.0f;
    uint32_t id = queue.AddOrder(o);
    assert(queue.QueueSize() == 1);

    queue.RemoveOrder(id);
    assert(queue.QueueSize() == 0);
    assert(queue.GetOrder(id) == nullptr);
    std::cout << "[PASS] test_build_queue_remove" << std::endl;
}

void test_build_queue_priority_sort() {
    BuildQueue queue;
    BuildOrder low;
    low.type = BuildOrderType::Construct;
    low.totalTimeSeconds = 10.0f;
    low.priority = 50;
    queue.AddOrder(low);

    BuildOrder high;
    high.type = BuildOrderType::Construct;
    high.totalTimeSeconds = 10.0f;
    high.priority = 200;
    queue.AddOrder(high);

    BuildOrder mid;
    mid.type = BuildOrderType::Construct;
    mid.totalTimeSeconds = 10.0f;
    mid.priority = 128;
    queue.AddOrder(mid);

    auto sorted = queue.GetOrdersByPriority();
    assert(sorted.size() == 3);
    assert(sorted[0].priority >= sorted[1].priority);
    assert(sorted[1].priority >= sorted[2].priority);
    assert(sorted[0].priority == 200);
    assert(sorted[2].priority == 50);
    std::cout << "[PASS] test_build_queue_priority_sort" << std::endl;
}

void test_build_queue_total_remaining() {
    BuildQueue queue;
    BuildOrder o1;
    o1.type = BuildOrderType::Construct;
    o1.totalTimeSeconds = 10.0f;
    queue.AddOrder(o1);

    BuildOrder o2;
    o2.type = BuildOrderType::Upgrade;
    o2.totalTimeSeconds = 20.0f;
    queue.AddOrder(o2);

    queue.Tick(5.0f);
    float remaining = queue.TotalRemainingTime();
    assert(std::fabs(remaining - 20.0f) < 0.001f);
    std::cout << "[PASS] test_build_queue_total_remaining" << std::endl;
}

// ============================================================
// PlanetaryBase tests
// ============================================================

void test_planetary_add_zone() {
    PlanetaryBase base;
    base.Init(PlanetaryBaseConfig::Default());

    uint32_t id = base.AddZone(BaseZoneType::Landing, 0.0f, 0.0f, 10.0f);
    assert(id != 0);
    assert(base.ZoneCount() == 1);
    const BaseZone* z = base.GetZone(id);
    assert(z != nullptr);
    assert(z->type == BaseZoneType::Landing);
    std::cout << "[PASS] test_planetary_add_zone" << std::endl;
}

void test_planetary_spacing_validation() {
    PlanetaryBase base;
    PlanetaryBaseConfig cfg = PlanetaryBaseConfig::Default();
    cfg.minZoneSpacing = 5.0f;
    base.Init(cfg);

    uint32_t id1 = base.AddZone(BaseZoneType::Landing, 0.0f, 0.0f, 10.0f);
    assert(id1 != 0);

    uint32_t id2 = base.AddZone(BaseZoneType::Habitat, 1.0f, 1.0f, 10.0f);
    assert(id2 == 0);
    assert(base.ZoneCount() == 1);
    std::cout << "[PASS] test_planetary_spacing_validation" << std::endl;
}

void test_planetary_has_required_zones() {
    PlanetaryBase base;
    PlanetaryBaseConfig cfg = PlanetaryBaseConfig::Default();
    cfg.minZoneSpacing = 0.0f;
    base.Init(cfg);

    assert(!base.HasRequiredZones());

    base.AddZone(BaseZoneType::Landing, 0.0f, 0.0f, 5.0f);
    assert(!base.HasRequiredZones());

    base.AddZone(BaseZoneType::Power, 50.0f, 0.0f, 5.0f);
    assert(!base.HasRequiredZones());

    base.AddZone(BaseZoneType::Habitat, 100.0f, 0.0f, 5.0f);
    assert(base.HasRequiredZones());
    std::cout << "[PASS] test_planetary_has_required_zones" << std::endl;
}

void test_planetary_generate() {
    PlanetaryBase base;
    PlanetaryBaseConfig cfg = PlanetaryBaseConfig::Default();
    base.Init(cfg);

    base.Generate(12345);
    assert(base.ZoneCount() > 0);
    assert(base.HasRequiredZones());
    std::cout << "[PASS] test_planetary_generate" << std::endl;
}

void test_planetary_upgrade_zone() {
    PlanetaryBase base;
    base.Init(PlanetaryBaseConfig::Default());

    uint32_t id = base.AddZone(BaseZoneType::Mining, 0.0f, 0.0f, 10.0f);
    assert(id != 0);
    const BaseZone* z = base.GetZone(id);
    assert(z->level == 1);

    base.UpgradeZone(id);
    z = base.GetZone(id);
    assert(z->level == 2);

    base.UpgradeZone(id);
    base.UpgradeZone(id);
    base.UpgradeZone(id);
    z = base.GetZone(id);
    assert(z->level == 5);

    base.UpgradeZone(id);
    z = base.GetZone(id);
    assert(z->level == 5);
    std::cout << "[PASS] test_planetary_upgrade_zone" << std::endl;
}

void test_planetary_clear() {
    PlanetaryBase base;
    PlanetaryBaseConfig cfg = PlanetaryBaseConfig::Default();
    cfg.minZoneSpacing = 0.0f;
    base.Init(cfg);

    base.AddZone(BaseZoneType::Landing, 0.0f, 0.0f, 5.0f);
    base.AddZone(BaseZoneType::Power, 50.0f, 0.0f, 5.0f);
    assert(base.ZoneCount() == 2);

    base.Clear();
    assert(base.ZoneCount() == 0);
    std::cout << "[PASS] test_planetary_clear" << std::endl;
}

// ============================================================
// ModuleScaling tests
// ============================================================

void test_scaling_size_multiplier() {
    assert(std::fabs(ModuleScaling::SizeMultiplier(ModuleSize::XS) - 0.5f)  < 0.001f);
    assert(std::fabs(ModuleScaling::SizeMultiplier(ModuleSize::S)  - 1.0f)  < 0.001f);
    assert(std::fabs(ModuleScaling::SizeMultiplier(ModuleSize::M)  - 3.0f)  < 0.001f);
    assert(std::fabs(ModuleScaling::SizeMultiplier(ModuleSize::L)  - 5.0f)  < 0.001f);
    assert(std::fabs(ModuleScaling::SizeMultiplier(ModuleSize::XL) - 10.0f) < 0.001f);
    std::cout << "[PASS] test_scaling_size_multiplier" << std::endl;
}

void test_scaling_cost_multiplier() {
    assert(std::fabs(ModuleScaling::CostMultiplier(ModuleSize::XS) - 0.3f)  < 0.001f);
    assert(std::fabs(ModuleScaling::CostMultiplier(ModuleSize::S)  - 1.0f)  < 0.001f);
    assert(std::fabs(ModuleScaling::CostMultiplier(ModuleSize::M)  - 2.5f)  < 0.001f);
    assert(std::fabs(ModuleScaling::CostMultiplier(ModuleSize::L)  - 5.0f)  < 0.001f);
    assert(std::fabs(ModuleScaling::CostMultiplier(ModuleSize::XL) - 12.0f) < 0.001f);
    std::cout << "[PASS] test_scaling_cost_multiplier" << std::endl;
}

void test_scaling_power_draw() {
    assert(std::fabs(ModuleScaling::PowerDraw(ModuleSize::XS) - 1.0f)  < 0.001f);
    assert(std::fabs(ModuleScaling::PowerDraw(ModuleSize::S)  - 2.0f)  < 0.001f);
    assert(std::fabs(ModuleScaling::PowerDraw(ModuleSize::M)  - 5.0f)  < 0.001f);
    assert(std::fabs(ModuleScaling::PowerDraw(ModuleSize::L)  - 10.0f) < 0.001f);
    assert(std::fabs(ModuleScaling::PowerDraw(ModuleSize::XL) - 25.0f) < 0.001f);
    std::cout << "[PASS] test_scaling_power_draw" << std::endl;
}

void test_scaling_fits_in_slot() {
    assert(ModuleScaling::FitsInSlot(ModuleSize::XS, ModuleSize::XS));
    assert(ModuleScaling::FitsInSlot(ModuleSize::XS, ModuleSize::S));
    assert(ModuleScaling::FitsInSlot(ModuleSize::S, ModuleSize::M));
    assert(ModuleScaling::FitsInSlot(ModuleSize::M, ModuleSize::XL));
    assert(ModuleScaling::FitsInSlot(ModuleSize::XL, ModuleSize::XL));
    assert(!ModuleScaling::FitsInSlot(ModuleSize::L, ModuleSize::S));
    assert(!ModuleScaling::FitsInSlot(ModuleSize::XL, ModuleSize::L));
    assert(!ModuleScaling::FitsInSlot(ModuleSize::M, ModuleSize::S));
    std::cout << "[PASS] test_scaling_fits_in_slot" << std::endl;
}

void test_scaling_convert_to_small() {
    assert(ModuleScaling::ConvertToSmallUnits(ModuleSize::S, 1)  == 1);
    assert(ModuleScaling::ConvertToSmallUnits(ModuleSize::M, 1)  == 3);
    assert(ModuleScaling::ConvertToSmallUnits(ModuleSize::L, 1)  == 5);
    assert(ModuleScaling::ConvertToSmallUnits(ModuleSize::XL, 1) == 10);
    assert(ModuleScaling::ConvertToSmallUnits(ModuleSize::M, 4)  == 12);
    assert(ModuleScaling::ConvertToSmallUnits(ModuleSize::L, 2)  == 10);
    std::cout << "[PASS] test_scaling_convert_to_small" << std::endl;
}
