#include <iostream>
#include <cassert>
#include <string>
#include <vector>
#include <memory>

#include "../engine/tools/EditorToolRegistry.h"
#include "../engine/tools/SimulationStepTool.h"
#include "../engine/tools/EnvironmentControlTool.h"
#include "../engine/tools/BatchTransformTool.h"
#include "../engine/tools/NPCSpawnerTool.h"
#include "../engine/tools/LiveEditTool.h"
#include "../engine/tools/MaterialOverrideTool.h"
#include "../engine/tools/PCGSnapshotTool.h"
#include "../engine/tools/EntityInspectorTool.h"
#include "../engine/tools/PrefabPlacementTool.h"
#include "../engine/tools/AnimationPreviewTool.h"

using namespace atlas::tools;

// ============================================================
// EditorToolRegistry Tests
// ============================================================

void test_registry_empty() {
    EditorToolRegistry reg;
    assert(reg.Count() == 0);
    assert(reg.ActiveTool() == nullptr);
    assert(reg.ToolNames().empty());
    std::cout << "[PASS] test_registry_empty" << std::endl;
}

void test_registry_register_and_find() {
    EditorToolRegistry reg;
    reg.Register(std::make_unique<SimulationStepTool>());
    reg.Register(std::make_unique<EnvironmentControlTool>());
    assert(reg.Count() == 2);

    assert(reg.FindByName("Simulation Step") != nullptr);
    assert(reg.FindByName("Environment Control") != nullptr);
    assert(reg.FindByName("NonExistent") == nullptr);

    auto names = reg.ToolNames();
    assert(names.size() == 2);
    assert(names[0] == "Simulation Step");
    assert(names[1] == "Environment Control");
    std::cout << "[PASS] test_registry_register_and_find" << std::endl;
}

void test_registry_activate_deactivate() {
    EditorToolRegistry reg;
    reg.Register(std::make_unique<SimulationStepTool>());
    reg.Register(std::make_unique<EnvironmentControlTool>());

    assert(reg.Activate("Simulation Step"));
    assert(reg.ActiveTool() != nullptr);
    assert(reg.ActiveTool()->Name() == "Simulation Step");
    assert(reg.ActiveTool()->IsActive());

    // Activating another tool deactivates the first
    assert(reg.Activate("Environment Control"));
    assert(reg.ActiveTool()->Name() == "Environment Control");
    assert(!reg.FindByName("Simulation Step")->IsActive());

    reg.DeactivateCurrent();
    assert(reg.ActiveTool() == nullptr);
    std::cout << "[PASS] test_registry_activate_deactivate" << std::endl;
}

void test_registry_activate_nonexistent() {
    EditorToolRegistry reg;
    reg.Register(std::make_unique<SimulationStepTool>());
    assert(!reg.Activate("Ghost Tool"));
    assert(reg.ActiveTool() == nullptr);
    std::cout << "[PASS] test_registry_activate_nonexistent" << std::endl;
}

void test_registry_update_active() {
    EditorToolRegistry reg;
    reg.Register(std::make_unique<SimulationStepTool>());
    reg.Activate("Simulation Step");

    auto* tool = dynamic_cast<SimulationStepTool*>(reg.ActiveTool());
    assert(tool != nullptr);
    assert(tool->StepCount() == 0);

    reg.UpdateActive(0.016f);
    assert(tool->StepCount() == 1);
    std::cout << "[PASS] test_registry_update_active" << std::endl;
}

void test_registry_clear() {
    EditorToolRegistry reg;
    reg.Register(std::make_unique<SimulationStepTool>());
    reg.Register(std::make_unique<EnvironmentControlTool>());
    reg.Activate("Simulation Step");

    reg.Clear();
    assert(reg.Count() == 0);
    assert(reg.ActiveTool() == nullptr);
    std::cout << "[PASS] test_registry_clear" << std::endl;
}

void test_registry_get_by_index() {
    EditorToolRegistry reg;
    reg.Register(std::make_unique<SimulationStepTool>());
    reg.Register(std::make_unique<EnvironmentControlTool>());

    assert(reg.Get(0) != nullptr);
    assert(reg.Get(0)->Name() == "Simulation Step");
    assert(reg.Get(1)->Name() == "Environment Control");
    assert(reg.Get(2) == nullptr);
    std::cout << "[PASS] test_registry_get_by_index" << std::endl;
}

// ============================================================
// SimulationStepTool Tests
// ============================================================

void test_sim_step_initial() {
    SimulationStepTool tool;
    assert(!tool.IsActive());
    assert(tool.StepCount() == 0);
    std::cout << "[PASS] test_sim_step_initial" << std::endl;
}

void test_sim_step_activate_update() {
    SimulationStepTool tool;
    tool.Activate();
    assert(tool.IsActive());
    assert(!tool.IsPaused());

    tool.Update(0.016f);
    tool.Update(0.016f);
    assert(tool.StepCount() == 2);
    assert(tool.AccumulatedTime() > 0.031f && tool.AccumulatedTime() < 0.033f);
    std::cout << "[PASS] test_sim_step_activate_update" << std::endl;
}

void test_sim_step_pause_resume() {
    SimulationStepTool tool;
    tool.Activate();

    tool.SetPaused(true);
    assert(tool.IsPaused());
    tool.Update(0.016f);
    assert(tool.StepCount() == 0);  // Paused, no steps

    tool.SetPaused(false);
    tool.Update(0.016f);
    assert(tool.StepCount() == 1);
    std::cout << "[PASS] test_sim_step_pause_resume" << std::endl;
}

void test_sim_step_request_step() {
    SimulationStepTool tool;
    tool.Activate();
    tool.SetPaused(true);

    tool.RequestStep();
    tool.Update(0.016f);
    assert(tool.StepCount() == 1);

    // Next update should be blocked (paused, no step requested)
    tool.Update(0.016f);
    assert(tool.StepCount() == 1);
    std::cout << "[PASS] test_sim_step_request_step" << std::endl;
}

void test_sim_step_speed_multiplier() {
    SimulationStepTool tool;
    tool.Activate();
    tool.SetSpeedMultiplier(2.0f);
    assert(tool.SpeedMultiplier() == 2.0f);

    tool.Update(1.0f);
    assert(tool.AccumulatedTime() > 1.99f && tool.AccumulatedTime() < 2.01f);
    std::cout << "[PASS] test_sim_step_speed_multiplier" << std::endl;
}

void test_sim_step_deactivate_resets() {
    SimulationStepTool tool;
    tool.Activate();
    tool.Update(0.016f);
    tool.SetPaused(true);
    tool.Deactivate();

    assert(!tool.IsActive());
    assert(!tool.IsPaused());
    std::cout << "[PASS] test_sim_step_deactivate_resets" << std::endl;
}

// ============================================================
// EnvironmentControlTool Tests
// ============================================================

void test_env_control_defaults() {
    EnvironmentControlTool tool;
    tool.Activate();
    assert(tool.GravityY() < -9.0f);  // default ~-9.81
    assert(tool.AtmosphereDensity() == 1.0f);
    assert(tool.TimeOfDay() == 12.0f);
    std::cout << "[PASS] test_env_control_defaults" << std::endl;
}

void test_env_control_gravity() {
    EnvironmentControlTool tool;
    tool.Activate();
    tool.SetGravity(0.0f, -1.62f, 0.0f);  // Moon gravity
    assert(tool.GravityX() == 0.0f);
    assert(tool.GravityY() == -1.62f);
    assert(tool.GravityZ() == 0.0f);
    std::cout << "[PASS] test_env_control_gravity" << std::endl;
}

void test_env_control_wind() {
    EnvironmentControlTool tool;
    tool.Activate();
    tool.SetWind(5.0f, 0.0f, -3.0f);
    assert(tool.WindX() == 5.0f);
    assert(tool.WindZ() == -3.0f);
    std::cout << "[PASS] test_env_control_wind" << std::endl;
}

void test_env_control_atmosphere() {
    EnvironmentControlTool tool;
    tool.Activate();
    tool.SetAtmosphereDensity(0.5f);
    assert(tool.AtmosphereDensity() == 0.5f);

    // Clamped to valid range
    tool.SetAtmosphereDensity(-1.0f);
    assert(tool.AtmosphereDensity() == 0.0f);
    std::cout << "[PASS] test_env_control_atmosphere" << std::endl;
}

void test_env_control_time_of_day() {
    EnvironmentControlTool tool;
    tool.Activate();
    tool.SetTimeOfDay(6.0f);
    assert(tool.TimeOfDay() == 6.0f);

    // Wraps around
    tool.SetTimeOfDay(25.0f);
    assert(tool.TimeOfDay() >= 0.0f && tool.TimeOfDay() < 24.0f);

    tool.SetTimeOfDay(-1.0f);
    assert(tool.TimeOfDay() >= 0.0f && tool.TimeOfDay() < 24.0f);
    std::cout << "[PASS] test_env_control_time_of_day" << std::endl;
}

void test_env_control_elapsed() {
    EnvironmentControlTool tool;
    tool.Activate();
    tool.Update(0.5f);
    tool.Update(0.5f);
    assert(tool.ElapsedTime() > 0.99f && tool.ElapsedTime() < 1.01f);
    std::cout << "[PASS] test_env_control_elapsed" << std::endl;
}

// ============================================================
// BatchTransformTool Tests
// ============================================================

void test_batch_select_deselect() {
    BatchTransformTool tool;
    tool.Activate();
    tool.Select(1);
    tool.Select(2);
    tool.Select(3);
    assert(tool.SelectionCount() == 3);
    assert(tool.IsSelected(2));

    tool.Deselect(2);
    assert(tool.SelectionCount() == 2);
    assert(!tool.IsSelected(2));
    std::cout << "[PASS] test_batch_select_deselect" << std::endl;
}

void test_batch_no_duplicates() {
    BatchTransformTool tool;
    tool.Activate();
    tool.Select(1);
    tool.Select(1);
    assert(tool.SelectionCount() == 1);
    std::cout << "[PASS] test_batch_no_duplicates" << std::endl;
}

void test_batch_apply_translate() {
    BatchTransformTool tool;
    tool.Activate();
    tool.Select(10);
    tool.Select(20);

    tool.ApplyTranslate(1.0f, 2.0f, 3.0f);
    assert(tool.OperationCount() == 1);
    assert(tool.LastTranslateX() == 1.0f);
    assert(tool.LastTranslateY() == 2.0f);
    assert(tool.LastTranslateZ() == 3.0f);
    std::cout << "[PASS] test_batch_apply_translate" << std::endl;
}

void test_batch_ops_require_selection() {
    BatchTransformTool tool;
    tool.Activate();
    // No selection — operations should be no-ops
    tool.ApplyTranslate(1.0f, 0.0f, 0.0f);
    assert(tool.OperationCount() == 0);

    tool.Select(1);
    tool.ApplyScale(2.0f, 2.0f, 2.0f);
    assert(tool.OperationCount() == 1);

    tool.ApplyRotate(90.0f);
    assert(tool.OperationCount() == 2);
    std::cout << "[PASS] test_batch_ops_require_selection" << std::endl;
}

void test_batch_clear_selection() {
    BatchTransformTool tool;
    tool.Activate();
    tool.Select(1);
    tool.Select(2);
    tool.ClearSelection();
    assert(tool.SelectionCount() == 0);
    std::cout << "[PASS] test_batch_clear_selection" << std::endl;
}

void test_batch_deactivate_clears() {
    BatchTransformTool tool;
    tool.Activate();
    tool.Select(1);
    tool.Deactivate();
    assert(tool.SelectionCount() == 0);
    std::cout << "[PASS] test_batch_deactivate_clears" << std::endl;
}

// ============================================================
// NPCSpawnerTool Tests
// ============================================================

void test_npc_spawner_default_template() {
    NPCSpawnerTool tool;
    tool.Activate();
    assert(tool.CurrentTemplate() == "default_npc");
    std::cout << "[PASS] test_npc_spawner_default_template" << std::endl;
}

void test_npc_spawner_set_template() {
    NPCSpawnerTool tool;
    tool.Activate();
    tool.SetTemplate("guard_patrol");
    assert(tool.CurrentTemplate() == "guard_patrol");
    std::cout << "[PASS] test_npc_spawner_set_template" << std::endl;
}

void test_npc_spawner_spawn() {
    NPCSpawnerTool tool;
    tool.Activate();
    tool.SetTemplate("merchant");

    uint32_t id = tool.Spawn(10.0f, 0.0f, 20.0f);
    assert(id > 0);
    assert(tool.SpawnCount() == 1);

    auto& history = tool.SpawnHistory();
    assert(history[0].templateName == "merchant");
    assert(history[0].posX == 10.0f);
    assert(history[0].posZ == 20.0f);
    std::cout << "[PASS] test_npc_spawner_spawn" << std::endl;
}

void test_npc_spawner_undo() {
    NPCSpawnerTool tool;
    tool.Activate();
    tool.Spawn(0, 0, 0);
    tool.Spawn(1, 0, 0);
    assert(tool.SpawnCount() == 2);

    assert(tool.UndoLastSpawn());
    assert(tool.SpawnCount() == 1);

    assert(tool.UndoLastSpawn());
    assert(tool.SpawnCount() == 0);

    assert(!tool.UndoLastSpawn());  // nothing left
    std::cout << "[PASS] test_npc_spawner_undo" << std::endl;
}

void test_npc_spawner_unique_ids() {
    NPCSpawnerTool tool;
    tool.Activate();
    uint32_t a = tool.Spawn(0, 0, 0);
    uint32_t b = tool.Spawn(1, 1, 1);
    assert(a != b);
    std::cout << "[PASS] test_npc_spawner_unique_ids" << std::endl;
}

// ============================================================
// LiveEditTool Tests
// ============================================================

void test_live_edit_apply() {
    LiveEditTool tool;
    tool.Activate();

    tool.ApplyEdit(42, "Transform", "position.x", "0.0", "10.0");
    assert(tool.EditCount() == 1);

    auto& edits = tool.EditHistory();
    assert(edits[0].entityId == 42);
    assert(edits[0].component == "Transform");
    assert(edits[0].field == "position.x");
    assert(edits[0].oldValue == "0.0");
    assert(edits[0].newValue == "10.0");
    std::cout << "[PASS] test_live_edit_apply" << std::endl;
}

void test_live_edit_undo() {
    LiveEditTool tool;
    tool.Activate();
    tool.ApplyEdit(1, "A", "b", "old", "new");
    tool.ApplyEdit(2, "C", "d", "old2", "new2");

    assert(tool.UndoLastEdit());
    assert(tool.EditCount() == 1);
    assert(tool.EditHistory()[0].entityId == 1);

    assert(tool.UndoLastEdit());
    assert(tool.EditCount() == 0);
    assert(!tool.UndoLastEdit());
    std::cout << "[PASS] test_live_edit_undo" << std::endl;
}

void test_live_edit_clear() {
    LiveEditTool tool;
    tool.Activate();
    tool.ApplyEdit(1, "A", "b", "0", "1");
    tool.ClearHistory();
    assert(tool.EditCount() == 0);
    std::cout << "[PASS] test_live_edit_clear" << std::endl;
}

void test_live_edit_elapsed() {
    LiveEditTool tool;
    tool.Activate();
    tool.Update(0.5f);
    tool.Update(0.3f);
    assert(tool.ElapsedTime() > 0.79f && tool.ElapsedTime() < 0.81f);
    std::cout << "[PASS] test_live_edit_elapsed" << std::endl;
}

// ============================================================
// MaterialOverrideTool Tests
// ============================================================

void test_material_override_target() {
    MaterialOverrideTool tool;
    tool.Activate();
    assert(tool.Target() == 0);

    tool.SetTarget(99);
    assert(tool.Target() == 99);
    std::cout << "[PASS] test_material_override_target" << std::endl;
}

void test_material_override_params() {
    MaterialOverrideTool tool;
    tool.Activate();
    tool.SetTarget(1);

    tool.SetParam("roughness", 0.5f);
    tool.SetParam("metallic", 1.0f);
    assert(tool.ParamCount() == 2);
    assert(tool.HasParam("roughness"));
    assert(tool.GetParam("roughness") == 0.5f);
    assert(tool.GetParam("metallic") == 1.0f);

    // Update existing param
    tool.SetParam("roughness", 0.8f);
    assert(tool.ParamCount() == 2);
    assert(tool.GetParam("roughness") == 0.8f);
    std::cout << "[PASS] test_material_override_params" << std::endl;
}

void test_material_override_reset() {
    MaterialOverrideTool tool;
    tool.Activate();
    tool.SetTarget(1);
    tool.SetParam("color_r", 1.0f);
    tool.ResetOverrides();
    assert(tool.ParamCount() == 0);
    std::cout << "[PASS] test_material_override_reset" << std::endl;
}

void test_material_override_target_change_clears() {
    MaterialOverrideTool tool;
    tool.Activate();
    tool.SetTarget(1);
    tool.SetParam("roughness", 0.5f);

    tool.SetTarget(2);  // Changing target clears params
    assert(tool.ParamCount() == 0);
    assert(tool.Target() == 2);
    std::cout << "[PASS] test_material_override_target_change_clears" << std::endl;
}

void test_material_override_unknown_param() {
    MaterialOverrideTool tool;
    tool.Activate();
    assert(!tool.HasParam("ghost"));
    assert(tool.GetParam("ghost") == 0.0f);
    std::cout << "[PASS] test_material_override_unknown_param" << std::endl;
}

// ============================================================
// PCGSnapshotTool Tests
// ============================================================

void test_pcg_snapshot_take() {
    PCGSnapshotTool tool;
    tool.Activate();

    uint32_t id = tool.TakeSnapshot("forest_v1", 12345, 1);
    assert(id > 0);
    assert(tool.SnapshotCount() == 1);

    auto* snap = tool.GetSnapshot(id);
    assert(snap != nullptr);
    assert(snap->name == "forest_v1");
    assert(snap->seed == 12345);
    assert(snap->version == 1);
    std::cout << "[PASS] test_pcg_snapshot_take" << std::endl;
}

void test_pcg_snapshot_restore() {
    PCGSnapshotTool tool;
    tool.Activate();

    uint32_t id1 = tool.TakeSnapshot("A", 111, 1);
    tool.TakeSnapshot("B", 222, 2);

    assert(tool.Restore(id1));
    assert(tool.LastRestoredId() == id1);

    assert(!tool.Restore(999));  // nonexistent
    assert(tool.LastRestoredId() == id1);  // unchanged
    std::cout << "[PASS] test_pcg_snapshot_restore" << std::endl;
}

void test_pcg_snapshot_clear() {
    PCGSnapshotTool tool;
    tool.Activate();
    tool.TakeSnapshot("X", 0, 0);
    tool.ClearSnapshots();
    assert(tool.SnapshotCount() == 0);
    assert(tool.LastRestoredId() == 0);
    std::cout << "[PASS] test_pcg_snapshot_clear" << std::endl;
}

void test_pcg_snapshot_multiple() {
    PCGSnapshotTool tool;
    tool.Activate();
    tool.TakeSnapshot("A", 1, 1);
    tool.TakeSnapshot("B", 2, 2);
    tool.TakeSnapshot("C", 3, 3);
    assert(tool.SnapshotCount() == 3);

    auto& all = tool.AllSnapshots();
    assert(all[0].name == "A");
    assert(all[2].seed == 3);
    std::cout << "[PASS] test_pcg_snapshot_multiple" << std::endl;
}

// ============================================================
// EntityInspectorTool Tests
// ============================================================

void test_entity_inspector_inspect() {
    EntityInspectorTool tool;
    tool.Activate();

    tool.Inspect(42);
    assert(tool.InspectedEntity() == 42);
    assert(tool.ComponentCount() == 0);

    tool.AddComponent("Transform", {{"x", "10"}, {"y", "20"}, {"z", "0"}});
    tool.AddComponent("Health", {{"hp", "100"}, {"max", "100"}});
    assert(tool.ComponentCount() == 2);
    assert(tool.Components()[0].name == "Transform");
    assert(tool.Components()[0].fields.size() == 3);
    assert(tool.Components()[1].name == "Health");
    std::cout << "[PASS] test_entity_inspector_inspect" << std::endl;
}

void test_entity_inspector_clear() {
    EntityInspectorTool tool;
    tool.Activate();
    tool.Inspect(1);
    tool.AddComponent("A", {});
    tool.ClearInspection();
    assert(tool.InspectedEntity() == 0);
    assert(tool.ComponentCount() == 0);
    std::cout << "[PASS] test_entity_inspector_clear" << std::endl;
}

void test_entity_inspector_reinspect() {
    EntityInspectorTool tool;
    tool.Activate();
    tool.Inspect(1);
    tool.AddComponent("A", {});
    tool.Inspect(2);  // Should clear previous components
    assert(tool.InspectedEntity() == 2);
    assert(tool.ComponentCount() == 0);
    std::cout << "[PASS] test_entity_inspector_reinspect" << std::endl;
}

void test_entity_inspector_deactivate_clears() {
    EntityInspectorTool tool;
    tool.Activate();
    tool.Inspect(5);
    tool.AddComponent("X", {});
    tool.Deactivate();
    assert(tool.InspectedEntity() == 0);
    assert(tool.ComponentCount() == 0);
    std::cout << "[PASS] test_entity_inspector_deactivate_clears" << std::endl;
}

// ============================================================
// PrefabPlacementTool Tests
// ============================================================

void test_prefab_catalogue() {
    PrefabPlacementTool tool;
    tool.Activate();
    tool.AddPrefab("wall_segment");
    tool.AddPrefab("door_frame");
    tool.AddPrefab("wall_segment");  // duplicate
    assert(tool.CatalogueSize() == 2);
    std::cout << "[PASS] test_prefab_catalogue" << std::endl;
}

void test_prefab_select_and_place() {
    PrefabPlacementTool tool;
    tool.Activate();
    tool.AddPrefab("tree_oak");
    tool.SelectPrefab("tree_oak");
    assert(tool.SelectedPrefab() == "tree_oak");

    uint32_t id = tool.Place(10.0f, 0.0f, 20.0f, 45.0f, 1.5f);
    assert(id > 0);
    assert(tool.PlacedCount() == 1);

    auto& placed = tool.PlacedInstances();
    assert(placed[0].prefabName == "tree_oak");
    assert(placed[0].posX == 10.0f);
    assert(placed[0].rotY == 45.0f);
    assert(placed[0].scale == 1.5f);
    std::cout << "[PASS] test_prefab_select_and_place" << std::endl;
}

void test_prefab_undo_placement() {
    PrefabPlacementTool tool;
    tool.Activate();
    tool.AddPrefab("rock");
    tool.SelectPrefab("rock");
    tool.Place(0, 0, 0);
    tool.Place(1, 1, 1);
    assert(tool.PlacedCount() == 2);

    assert(tool.UndoLastPlacement());
    assert(tool.PlacedCount() == 1);
    assert(!tool.PlacedInstances()[0].prefabName.empty());

    assert(tool.UndoLastPlacement());
    assert(tool.PlacedCount() == 0);
    assert(!tool.UndoLastPlacement());
    std::cout << "[PASS] test_prefab_undo_placement" << std::endl;
}

void test_prefab_unique_instance_ids() {
    PrefabPlacementTool tool;
    tool.Activate();
    tool.AddPrefab("box");
    tool.SelectPrefab("box");
    uint32_t a = tool.Place(0, 0, 0);
    uint32_t b = tool.Place(1, 0, 0);
    assert(a != b);
    std::cout << "[PASS] test_prefab_unique_instance_ids" << std::endl;
}

// ============================================================
// AnimationPreviewTool Tests
// ============================================================

void test_anim_preview_initial() {
    AnimationPreviewTool tool;
    tool.Activate();
    assert(tool.Target() == 0);
    assert(tool.CurrentClip().empty());
    assert(!tool.IsPlaying());
    assert(tool.PlaybackSpeed() == 1.0f);
    assert(tool.CurrentTime() == 0.0f);
    assert(tool.BlendWeight() == 1.0f);
    std::cout << "[PASS] test_anim_preview_initial" << std::endl;
}

void test_anim_preview_playback() {
    AnimationPreviewTool tool;
    tool.Activate();
    tool.SetTarget(10);
    tool.SetClip("walk");
    tool.SetPlaying(true);

    tool.Update(0.5f);
    assert(tool.CurrentTime() > 0.49f && tool.CurrentTime() < 0.51f);

    tool.Update(0.5f);
    assert(tool.CurrentTime() > 0.99f && tool.CurrentTime() < 1.01f);
    std::cout << "[PASS] test_anim_preview_playback" << std::endl;
}

void test_anim_preview_speed() {
    AnimationPreviewTool tool;
    tool.Activate();
    tool.SetClip("run");
    tool.SetPlaybackSpeed(2.0f);
    tool.SetPlaying(true);

    tool.Update(1.0f);
    assert(tool.CurrentTime() > 1.99f && tool.CurrentTime() < 2.01f);
    std::cout << "[PASS] test_anim_preview_speed" << std::endl;
}

void test_anim_preview_scrub() {
    AnimationPreviewTool tool;
    tool.Activate();
    tool.SetClip("idle");

    tool.Scrub(5.0f);
    assert(tool.CurrentTime() == 5.0f);

    // Clamp to >= 0
    tool.Scrub(-1.0f);
    assert(tool.CurrentTime() == 0.0f);
    std::cout << "[PASS] test_anim_preview_scrub" << std::endl;
}

void test_anim_preview_blend_weight() {
    AnimationPreviewTool tool;
    tool.Activate();

    tool.SetBlendWeight(0.5f);
    assert(tool.BlendWeight() == 0.5f);

    // Clamp to 0–1
    tool.SetBlendWeight(1.5f);
    assert(tool.BlendWeight() == 1.0f);
    tool.SetBlendWeight(-0.5f);
    assert(tool.BlendWeight() == 0.0f);
    std::cout << "[PASS] test_anim_preview_blend_weight" << std::endl;
}

void test_anim_preview_reset() {
    AnimationPreviewTool tool;
    tool.Activate();
    tool.SetTarget(5);
    tool.SetClip("jump");
    tool.SetPlaying(true);
    tool.Update(1.0f);

    tool.Reset();
    assert(tool.CurrentClip().empty());
    assert(!tool.IsPlaying());
    assert(tool.CurrentTime() == 0.0f);
    assert(tool.BlendWeight() == 1.0f);
    std::cout << "[PASS] test_anim_preview_reset" << std::endl;
}

void test_anim_preview_not_playing_no_advance() {
    AnimationPreviewTool tool;
    tool.Activate();
    tool.SetClip("idle");
    // Not playing
    tool.Update(1.0f);
    assert(tool.CurrentTime() == 0.0f);
    std::cout << "[PASS] test_anim_preview_not_playing_no_advance" << std::endl;
}

// ============================================================
// Full Registry Integration Test
// ============================================================

void test_registry_all_10_tools() {
    EditorToolRegistry reg;
    reg.Register(std::make_unique<SimulationStepTool>());
    reg.Register(std::make_unique<EnvironmentControlTool>());
    reg.Register(std::make_unique<BatchTransformTool>());
    reg.Register(std::make_unique<NPCSpawnerTool>());
    reg.Register(std::make_unique<LiveEditTool>());
    reg.Register(std::make_unique<MaterialOverrideTool>());
    reg.Register(std::make_unique<PCGSnapshotTool>());
    reg.Register(std::make_unique<EntityInspectorTool>());
    reg.Register(std::make_unique<PrefabPlacementTool>());
    reg.Register(std::make_unique<AnimationPreviewTool>());

    assert(reg.Count() == 10);

    // Cycle through each tool
    auto names = reg.ToolNames();
    for (const auto& name : names) {
        assert(reg.Activate(name));
        assert(reg.ActiveTool()->IsActive());
        reg.UpdateActive(0.016f);
    }

    reg.DeactivateCurrent();
    assert(reg.ActiveTool() == nullptr);

    std::cout << "[PASS] test_registry_all_10_tools" << std::endl;
}
