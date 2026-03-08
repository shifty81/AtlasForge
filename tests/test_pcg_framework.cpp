#include "../engine/procedural/DeterministicRNG.h"
#include "../engine/procedural/PCGDomain.h"
#include "../engine/procedural/PCGManager.h"
#include "../engine/procedural/PCGVerify.h"
#include "../engine/procedural/ConstraintSolver.h"
#include <cassert>
#include <iostream>
#include <cstring>
#include <set>

using namespace atlas::procedural;

// ============================================================
// DeterministicRNG tests
// ============================================================

void test_rng_deterministic() {
    DeterministicRNG a(12345);
    DeterministicRNG b(12345);
    for (int i = 0; i < 1000; ++i) {
        assert(a.Next() == b.Next());
    }
    std::cout << "[PASS] test_rng_deterministic" << std::endl;
}

void test_rng_different_seeds() {
    DeterministicRNG a(1);
    DeterministicRNG b(2);
    // Very unlikely to produce the same first 10 values.
    bool allSame = true;
    for (int i = 0; i < 10; ++i) {
        if (a.Next() != b.Next()) allSame = false;
    }
    assert(!allSame);
    std::cout << "[PASS] test_rng_different_seeds" << std::endl;
}

void test_rng_zero_seed_remapped() {
    DeterministicRNG rng(0);
    // Should still produce values (seed 0 is remapped to 1).
    uint64_t v = rng.Next();
    assert(v != 0);
    std::cout << "[PASS] test_rng_zero_seed_remapped" << std::endl;
}

void test_rng_float_range() {
    DeterministicRNG rng(42);
    for (int i = 0; i < 10000; ++i) {
        float f = rng.NextFloat();
        assert(f >= 0.0f && f < 1.0f);
    }
    std::cout << "[PASS] test_rng_float_range" << std::endl;
}

void test_rng_float_range_custom() {
    DeterministicRNG rng(99);
    for (int i = 0; i < 1000; ++i) {
        float f = rng.NextFloatRange(10.0f, 20.0f);
        assert(f >= 10.0f && f < 20.0f);
    }
    std::cout << "[PASS] test_rng_float_range_custom" << std::endl;
}

void test_rng_int_range() {
    DeterministicRNG rng(77);
    for (int i = 0; i < 1000; ++i) {
        int v = rng.NextIntRange(-5, 5);
        assert(v >= -5 && v <= 5);
    }
    std::cout << "[PASS] test_rng_int_range" << std::endl;
}

void test_rng_u32_range() {
    DeterministicRNG rng(55);
    for (int i = 0; i < 1000; ++i) {
        uint32_t v = rng.NextU32(10);
        assert(v < 10);
    }
    // Edge case: max 0 should return 0.
    assert(rng.NextU32(0) == 0);
    std::cout << "[PASS] test_rng_u32_range" << std::endl;
}

void test_rng_bool_distribution() {
    DeterministicRNG rng(123);
    int trueCount = 0;
    int total = 10000;
    for (int i = 0; i < total; ++i) {
        if (rng.NextBool(0.5f)) ++trueCount;
    }
    // Expect roughly 50% ± 5%.
    float ratio = static_cast<float>(trueCount) / static_cast<float>(total);
    assert(ratio > 0.45f && ratio < 0.55f);
    std::cout << "[PASS] test_rng_bool_distribution" << std::endl;
}

void test_rng_fork_independent() {
    DeterministicRNG parent(42);
    auto child1 = parent.Fork(1);
    auto child2 = parent.Fork(2);
    // Children with different salts produce different sequences.
    bool allSame = true;
    for (int i = 0; i < 10; ++i) {
        if (child1.Next() != child2.Next()) allSame = false;
    }
    assert(!allSame);
    std::cout << "[PASS] test_rng_fork_independent" << std::endl;
}

void test_rng_reseed() {
    DeterministicRNG rng(1);
    rng.Next(); rng.Next();
    rng.Seed(1);
    DeterministicRNG fresh(1);
    assert(rng.Next() == fresh.Next());
    std::cout << "[PASS] test_rng_reseed" << std::endl;
}

// ============================================================
// PCG Domain tests
// ============================================================

void test_pcg_domain_names() {
    assert(std::string(PCGDomainName(PCGDomain::Universe)) == "Universe");
    assert(std::string(PCGDomainName(PCGDomain::Ship)) == "Ship");
    assert(std::string(PCGDomainName(PCGDomain::Terrain)) == "Terrain");
    assert(std::string(PCGDomainName(static_cast<PCGDomain>(99))) == "Unknown");
    std::cout << "[PASS] test_pcg_domain_names" << std::endl;
}

void test_pcg_domain_count() {
    assert(static_cast<int>(PCGDomain::COUNT) == 16);
    std::cout << "[PASS] test_pcg_domain_count" << std::endl;
}

void test_pcg_context_child() {
    PCGContext parent(PCGDomain::Galaxy, SeedLevel::Universe, 12345);
    auto child = parent.Child(999);
    assert(child.domain == PCGDomain::Galaxy);
    assert(static_cast<uint8_t>(child.level) ==
           static_cast<uint8_t>(SeedLevel::Galaxy));
    assert(child.seed != parent.seed);
    std::cout << "[PASS] test_pcg_context_child" << std::endl;
}

void test_pcg_context_hierarchy_depth() {
    PCGContext ctx(PCGDomain::System, SeedLevel::Universe, 100);
    auto g = ctx.Child(1);
    auto s = g.Child(2);
    auto sec = s.Child(3);
    auto obj = sec.Child(4);
    // Each level should have a unique seed.
    std::set<uint64_t> seeds = {ctx.seed, g.seed, s.seed, sec.seed, obj.seed};
    assert(seeds.size() == 5);
    std::cout << "[PASS] test_pcg_context_hierarchy_depth" << std::endl;
}

// ============================================================
// PCGManager tests
// ============================================================

void test_pcg_manager_creation() {
    PCGManager mgr(42);
    assert(mgr.GetUniverseSeed() == 42);
    assert(mgr.GetVersion() == 1);
    assert(PCGManager::DomainCount() == 16);
    std::cout << "[PASS] test_pcg_manager_creation" << std::endl;
}

void test_pcg_manager_domain_isolation() {
    PCGManager mgr(42);
    uint64_t shipSeed = mgr.GetDomainSeed(PCGDomain::Ship);
    uint64_t npcSeed  = mgr.GetDomainSeed(PCGDomain::NPC);
    uint64_t lootSeed = mgr.GetDomainSeed(PCGDomain::Loot);
    // All domain seeds must be different.
    assert(shipSeed != npcSeed);
    assert(shipSeed != lootSeed);
    assert(npcSeed != lootSeed);
    std::cout << "[PASS] test_pcg_manager_domain_isolation" << std::endl;
}

void test_pcg_manager_deterministic() {
    PCGManager a(12345);
    PCGManager b(12345);
    for (int d = 0; d < PCGManager::DomainCount(); ++d) {
        assert(a.GetDomainSeed(static_cast<PCGDomain>(d)) ==
               b.GetDomainSeed(static_cast<PCGDomain>(d)));
    }
    std::cout << "[PASS] test_pcg_manager_deterministic" << std::endl;
}

void test_pcg_manager_reseed() {
    PCGManager mgr(1);
    uint64_t s1 = mgr.GetDomainSeed(PCGDomain::Ship);
    mgr.SetUniverseSeed(2);
    uint64_t s2 = mgr.GetDomainSeed(PCGDomain::Ship);
    assert(s1 != s2);
    // Re-seed back produces original.
    mgr.SetUniverseSeed(1);
    assert(mgr.GetDomainSeed(PCGDomain::Ship) == s1);
    std::cout << "[PASS] test_pcg_manager_reseed" << std::endl;
}

void test_pcg_manager_create_context() {
    PCGManager mgr(42);
    auto ctx = mgr.CreateContext(PCGDomain::Asteroid, SeedLevel::System, 7);
    assert(ctx.domain == PCGDomain::Asteroid);
    assert(ctx.seed != 0);
    // Same parameters should produce same context.
    auto ctx2 = mgr.CreateContext(PCGDomain::Asteroid, SeedLevel::System, 7);
    assert(ctx.seed == ctx2.seed);
    std::cout << "[PASS] test_pcg_manager_create_context" << std::endl;
}

void test_pcg_manager_version() {
    PCGManager mgr(1);
    assert(mgr.GetVersion() == 1);
    mgr.SetVersion(5);
    assert(mgr.GetVersion() == 5);
    std::cout << "[PASS] test_pcg_manager_version" << std::endl;
}

// ============================================================
// PCG Verification tests
// ============================================================

void test_pcg_verify_match() {
    PCGVerifier v;
    v.AddExpected(PCGDomain::Ship, 100, 0xDEAD);
    v.Submit(PCGDomain::Ship, 100, 0xDEAD);
    auto result = v.Verify();
    assert(result.allPassed);
    assert(result.PassedCount() == 1);
    assert(result.FailedCount() == 0);
    std::cout << "[PASS] test_pcg_verify_match" << std::endl;
}

void test_pcg_verify_mismatch() {
    PCGVerifier v;
    v.AddExpected(PCGDomain::NPC, 200, 0xBEEF);
    v.Submit(PCGDomain::NPC, 200, 0xCAFE);
    auto result = v.Verify();
    assert(!result.allPassed);
    assert(result.FailedCount() == 1);
    std::cout << "[PASS] test_pcg_verify_mismatch" << std::endl;
}

void test_pcg_verify_missing_submission() {
    PCGVerifier v;
    v.AddExpected(PCGDomain::Loot, 300, 0xAAAA);
    // No submission.
    auto result = v.Verify();
    assert(!result.allPassed);
    assert(result.records[0].actual == 0);
    std::cout << "[PASS] test_pcg_verify_missing_submission" << std::endl;
}

void test_pcg_verify_multiple() {
    PCGVerifier v;
    v.AddExpected(PCGDomain::Ship, 1, 100);
    v.AddExpected(PCGDomain::NPC, 2, 200);
    v.AddExpected(PCGDomain::Loot, 3, 300);
    v.Submit(PCGDomain::Ship, 1, 100);
    v.Submit(PCGDomain::NPC, 2, 200);
    v.Submit(PCGDomain::Loot, 3, 300);
    auto result = v.Verify();
    assert(result.allPassed);
    assert(result.records.size() == 3);
    std::cout << "[PASS] test_pcg_verify_multiple" << std::endl;
}

void test_pcg_verify_clear() {
    PCGVerifier v;
    v.AddExpected(PCGDomain::Ship, 1, 100);
    v.Submit(PCGDomain::Ship, 1, 100);
    assert(v.ExpectedCount() == 1);
    assert(v.SubmittedCount() == 1);
    v.Clear();
    assert(v.ExpectedCount() == 0);
    assert(v.SubmittedCount() == 0);
    std::cout << "[PASS] test_pcg_verify_clear" << std::endl;
}

void test_pcg_verify_hash_data() {
    const char* data = "hello";
    uint64_t h1 = PCGVerifier::HashData(data, 5);
    uint64_t h2 = PCGVerifier::HashData(data, 5);
    assert(h1 == h2);  // deterministic
    uint64_t h3 = PCGVerifier::HashData("world", 5);
    assert(h1 != h3);  // different data -> different hash
    std::cout << "[PASS] test_pcg_verify_hash_data" << std::endl;
}

// ============================================================
// Constraint Solver tests
// ============================================================

void test_constraint_solver_empty() {
    ConstraintSolver solver(42);
    ConstraintConfig cfg;
    auto result = solver.Solve(cfg);
    assert(result.itemCount == 0);
    std::cout << "[PASS] test_constraint_solver_empty" << std::endl;
}

void test_constraint_solver_single_item() {
    ConstraintSolver solver(42);
    solver.AddItem({"Laser", 25, 10});
    ConstraintConfig cfg;
    cfg.maxCapacity = 100;
    cfg.maxItems = 5;
    auto result = solver.Solve(cfg);
    // Should select the single item since it fits.
    assert(result.feasible);
    assert(result.itemCount >= 1);
    std::cout << "[PASS] test_constraint_solver_single_item" << std::endl;
}

void test_constraint_solver_capacity_limit() {
    ConstraintSolver solver(42);
    solver.AddItem({"A", 60, 10});
    solver.AddItem({"B", 60, 10});
    ConstraintConfig cfg;
    cfg.maxCapacity = 100;
    cfg.maxItems = 10;
    cfg.generations = 200;
    auto result = solver.Solve(cfg);
    // Both items cost 60, total 120 > 100.  Feasible means at most 1.
    if (result.feasible) {
        assert(result.totalCost <= cfg.maxCapacity);
    }
    std::cout << "[PASS] test_constraint_solver_capacity_limit" << std::endl;
}

void test_constraint_solver_deterministic() {
    auto run = [](uint64_t seed) {
        ConstraintSolver s(seed);
        s.AddItem({"X", 10, 5});
        s.AddItem({"Y", 20, 8});
        s.AddItem({"Z", 15, 7});
        ConstraintConfig cfg;
        cfg.maxCapacity = 30;
        cfg.maxItems = 2;
        cfg.populationSize = 20;
        cfg.generations = 50;
        return s.Solve(cfg);
    };
    auto r1 = run(777);
    auto r2 = run(777);
    assert(r1.selectedIndices == r2.selectedIndices);
    assert(r1.totalValue == r2.totalValue);
    std::cout << "[PASS] test_constraint_solver_deterministic" << std::endl;
}

void test_constraint_solver_item_count_limit() {
    ConstraintSolver solver(42);
    for (int i = 0; i < 20; ++i) {
        solver.AddItem({"Item" + std::to_string(i), 1, 5});
    }
    ConstraintConfig cfg;
    cfg.maxCapacity = 1000;
    cfg.maxItems = 3;
    cfg.generations = 200;
    auto result = solver.Solve(cfg);
    if (result.feasible) {
        assert(result.itemCount <= 3);
    }
    std::cout << "[PASS] test_constraint_solver_item_count_limit" << std::endl;
}

void test_constraint_solver_is_feasible() {
    ConstraintConfig cfg;
    cfg.maxCapacity = 50;
    cfg.maxItems = 3;

    ConstraintResult good;
    good.totalCost = 40;
    good.itemCount = 2;
    assert(ConstraintSolver::IsFeasible(good, cfg));

    ConstraintResult bad;
    bad.totalCost = 60;
    bad.itemCount = 2;
    assert(!ConstraintSolver::IsFeasible(bad, cfg));

    ConstraintResult tooMany;
    tooMany.totalCost = 10;
    tooMany.itemCount = 5;
    assert(!ConstraintSolver::IsFeasible(tooMany, cfg));

    std::cout << "[PASS] test_constraint_solver_is_feasible" << std::endl;
}

void test_constraint_solver_clear_items() {
    ConstraintSolver solver(1);
    solver.AddItem({"A", 10, 5});
    assert(solver.ItemCount() == 1);
    solver.ClearItems();
    assert(solver.ItemCount() == 0);
    std::cout << "[PASS] test_constraint_solver_clear_items" << std::endl;
}
