#include <iostream>
#include <cassert>
#include <string>
#include <vector>

#include "../engine/tools/ITool.h"
#include "../engine/tools/EditorCommandBus.h"
#include "../engine/tools/UndoableCommandBus.h"
#include "../engine/tools/EditorEventBus.h"
#include "../engine/tools/SceneBookmarkManager.h"
#include "../engine/tools/LayerTagSystem.h"

using namespace atlas::tools;

// ============================================================
// ITool Tests
// ============================================================

/// Concrete ITool implementation for testing.
class TestTool : public ITool {
public:
    explicit TestTool(std::string name) : m_name(std::move(name)) {}

    std::string Name() const override { return m_name; }

    void Activate() override { m_active = true; m_activateCount++; }
    void Deactivate() override { m_active = false; m_deactivateCount++; }
    void Update(float dt) override { m_totalDt += dt; m_updateCount++; }
    bool IsActive() const override { return m_active; }

    int m_activateCount = 0;
    int m_deactivateCount = 0;
    int m_updateCount = 0;
    float m_totalDt = 0.0f;

private:
    std::string m_name;
    bool m_active = false;
};

void test_itool_name() {
    TestTool tool("Test Tool");
    assert(tool.Name() == "Test Tool");
    std::cout << "[PASS] test_itool_name" << std::endl;
}

void test_itool_activate_deactivate() {
    TestTool tool("Toggle");
    assert(!tool.IsActive());

    tool.Activate();
    assert(tool.IsActive());
    assert(tool.m_activateCount == 1);

    tool.Deactivate();
    assert(!tool.IsActive());
    assert(tool.m_deactivateCount == 1);

    std::cout << "[PASS] test_itool_activate_deactivate" << std::endl;
}

void test_itool_update() {
    TestTool tool("Updater");
    tool.Activate();
    tool.Update(0.016f);
    tool.Update(0.016f);
    assert(tool.m_updateCount == 2);
    assert(tool.m_totalDt > 0.03f);

    std::cout << "[PASS] test_itool_update" << std::endl;
}

void test_itool_polymorphism() {
    std::vector<std::unique_ptr<ITool>> tools;
    tools.push_back(std::make_unique<TestTool>("A"));
    tools.push_back(std::make_unique<TestTool>("B"));

    for (auto& t : tools) t->Activate();
    for (auto& t : tools) assert(t->IsActive());

    assert(tools[0]->Name() == "A");
    assert(tools[1]->Name() == "B");

    std::cout << "[PASS] test_itool_polymorphism" << std::endl;
}

// ============================================================
// EditorCommandBus Tests
// ============================================================

void test_editor_cmd_bus_enqueue() {
    EditorCommandBus bus;
    assert(bus.PendingCount() == 0);

    EditorCommand cmd;
    cmd.type = EditorCommandType::SelectEntity;
    cmd.entityId = 42;
    bus.Enqueue(cmd);
    assert(bus.PendingCount() == 1);

    std::cout << "[PASS] test_editor_cmd_bus_enqueue" << std::endl;
}

void test_editor_cmd_bus_drain() {
    EditorCommandBus bus;
    EditorCommand c1; c1.type = EditorCommandType::CreateEntity;
    EditorCommand c2; c2.type = EditorCommandType::DeleteEntity;
    bus.Enqueue(c1);
    bus.Enqueue(c2);

    std::vector<EditorCommand> out;
    bus.Drain(out);
    assert(out.size() == 2);
    assert(bus.PendingCount() == 0);
    assert(out[0].type == EditorCommandType::CreateEntity);
    assert(out[1].type == EditorCommandType::DeleteEntity);

    std::cout << "[PASS] test_editor_cmd_bus_drain" << std::endl;
}

void test_editor_cmd_bus_clear() {
    EditorCommandBus bus;
    bus.Enqueue(EditorCommand{});
    bus.Enqueue(EditorCommand{});
    assert(bus.PendingCount() == 2);
    bus.Clear();
    assert(bus.PendingCount() == 0);

    std::cout << "[PASS] test_editor_cmd_bus_clear" << std::endl;
}

void test_editor_cmd_bus_dispatch() {
    EditorCommandBus bus;
    int called = 0;
    bus.RegisterHandler(EditorCommandType::SelectEntity,
                        [&](const EditorCommand& cmd) {
                            called++;
                            assert(cmd.entityId == 7);
                        });

    EditorCommand cmd;
    cmd.type = EditorCommandType::SelectEntity;
    cmd.entityId = 7;
    bus.Enqueue(cmd);
    bus.Dispatch();

    assert(called == 1);
    assert(bus.PendingCount() == 0);

    std::cout << "[PASS] test_editor_cmd_bus_dispatch" << std::endl;
}

void test_editor_cmd_bus_dispatch_unhandled() {
    EditorCommandBus bus;
    EditorCommand cmd;
    cmd.type = EditorCommandType::Custom;
    bus.Enqueue(cmd);

    // Should not crash — unhandled commands are silently ignored.
    bus.Dispatch();
    assert(bus.PendingCount() == 0);

    std::cout << "[PASS] test_editor_cmd_bus_dispatch_unhandled" << std::endl;
}

void test_editor_cmd_bus_move_enqueue() {
    EditorCommandBus bus;
    EditorCommand cmd;
    cmd.type = EditorCommandType::SetProperty;
    cmd.key = "position";
    cmd.value = "1,2,3";
    bus.Enqueue(std::move(cmd));
    assert(bus.PendingCount() == 1);

    std::vector<EditorCommand> out;
    bus.Drain(out);
    assert(out[0].key == "position");
    assert(out[0].value == "1,2,3");

    std::cout << "[PASS] test_editor_cmd_bus_move_enqueue" << std::endl;
}

// ============================================================
// UndoableCommandBus Tests
// ============================================================

class TestUndoableCmd : public UndoableEditorCommand {
public:
    int* counter;
    TestUndoableCmd(int* ctr, std::string name)
        : UndoableEditorCommand(std::move(name)), counter(ctr) {}

    void DoExecute() override { (*counter)++; }
    void DoUndo() override    { (*counter)--; }
};

void test_undoable_bus_execute() {
    UndoableCommandBus bus;
    int counter = 0;
    bus.Execute(std::make_unique<TestUndoableCmd>(&counter, "Inc"));
    assert(counter == 1);
    assert(bus.UndoCount() == 1);
    assert(bus.RedoCount() == 0);

    std::cout << "[PASS] test_undoable_bus_execute" << std::endl;
}

void test_undoable_bus_undo_redo() {
    UndoableCommandBus bus;
    int counter = 0;
    bus.Execute(std::make_unique<TestUndoableCmd>(&counter, "A"));
    bus.Execute(std::make_unique<TestUndoableCmd>(&counter, "B"));
    assert(counter == 2);

    bus.Undo();
    assert(counter == 1);
    assert(bus.CanRedo());

    bus.Redo();
    assert(counter == 2);

    std::cout << "[PASS] test_undoable_bus_undo_redo" << std::endl;
}

void test_undoable_bus_clear() {
    UndoableCommandBus bus;
    int counter = 0;
    bus.Execute(std::make_unique<TestUndoableCmd>(&counter, "X"));
    assert(bus.UndoCount() == 1);

    bus.Clear();
    assert(bus.UndoCount() == 0);
    assert(bus.RedoCount() == 0);

    std::cout << "[PASS] test_undoable_bus_clear" << std::endl;
}

void test_undoable_bus_last_executed() {
    UndoableCommandBus bus;
    assert(bus.LastExecuted() == nullptr);

    int counter = 0;
    bus.Execute(std::make_unique<TestUndoableCmd>(&counter, "MyCmd"));
    assert(bus.LastExecuted() != nullptr);
    assert(bus.LastExecuted()->Name() == "MyCmd");

    std::cout << "[PASS] test_undoable_bus_last_executed" << std::endl;
}

void test_undoable_bus_handler_notified() {
    UndoableCommandBus bus;
    int notifications = 0;
    bus.Bus().RegisterHandler(EditorCommandType::TransformEntity,
                              [&](const EditorCommand&) { notifications++; });

    int counter = 0;
    EditorCommand cmd;
    cmd.type = EditorCommandType::TransformEntity;

    // Use the constructor that accepts an EditorCommand.
    class NotifyCmd : public UndoableEditorCommand {
    public:
        int* ctr;
        NotifyCmd(int* c, EditorCommand ec)
            : UndoableEditorCommand("Transform", std::move(ec)), ctr(c) {}
        void DoExecute() override { (*ctr)++; }
        void DoUndo() override    { (*ctr)--; }
    };

    bus.Execute(std::make_unique<NotifyCmd>(&counter, cmd));
    assert(counter == 1);
    assert(notifications == 1);

    std::cout << "[PASS] test_undoable_bus_handler_notified" << std::endl;
}

// ============================================================
// EditorEventBus Tests
// ============================================================

void test_event_bus_subscribe_publish() {
    EditorEventBus bus;
    int received = 0;
    bus.Subscribe(EditorEventType::ToolActivated,
                  [&](const EditorEvent& e) {
                      received++;
                      assert(e.key == "TestTool");
                  });

    EditorEvent ev;
    ev.type = EditorEventType::ToolActivated;
    ev.key = "TestTool";
    bus.Publish(ev);
    assert(received == 1);

    std::cout << "[PASS] test_event_bus_subscribe_publish" << std::endl;
}

void test_event_bus_unsubscribe() {
    EditorEventBus bus;
    int received = 0;
    auto id = bus.Subscribe(EditorEventType::SelectionChanged,
                            [&](const EditorEvent&) { received++; });
    assert(bus.SubscriberCount() == 1);

    bus.Unsubscribe(id);
    assert(bus.SubscriberCount() == 0);

    EditorEvent ev;
    ev.type = EditorEventType::SelectionChanged;
    bus.Publish(ev);
    assert(received == 0);

    std::cout << "[PASS] test_event_bus_unsubscribe" << std::endl;
}

void test_event_bus_multiple_subscribers() {
    EditorEventBus bus;
    int countA = 0, countB = 0;
    bus.Subscribe(EditorEventType::EntityCreated,
                  [&](const EditorEvent&) { countA++; });
    bus.Subscribe(EditorEventType::EntityCreated,
                  [&](const EditorEvent&) { countB++; });

    EditorEvent ev;
    ev.type = EditorEventType::EntityCreated;
    bus.Publish(ev);
    assert(countA == 1);
    assert(countB == 1);

    std::cout << "[PASS] test_event_bus_multiple_subscribers" << std::endl;
}

void test_event_bus_type_filtering() {
    EditorEventBus bus;
    int toolCount = 0, entityCount = 0;
    bus.Subscribe(EditorEventType::ToolActivated,
                  [&](const EditorEvent&) { toolCount++; });
    bus.Subscribe(EditorEventType::EntityDeleted,
                  [&](const EditorEvent&) { entityCount++; });

    EditorEvent ev;
    ev.type = EditorEventType::ToolActivated;
    bus.Publish(ev);
    assert(toolCount == 1);
    assert(entityCount == 0);

    ev.type = EditorEventType::EntityDeleted;
    bus.Publish(ev);
    assert(toolCount == 1);
    assert(entityCount == 1);

    std::cout << "[PASS] test_event_bus_type_filtering" << std::endl;
}

void test_event_bus_clear() {
    EditorEventBus bus;
    bus.Subscribe(EditorEventType::Custom, [](const EditorEvent&) {});
    bus.Subscribe(EditorEventType::Custom, [](const EditorEvent&) {});
    assert(bus.SubscriberCount() == 2);
    bus.Clear();
    assert(bus.SubscriberCount() == 0);

    std::cout << "[PASS] test_event_bus_clear" << std::endl;
}

// ============================================================
// SceneBookmarkManager Tests
// ============================================================

void test_bookmark_save_get() {
    SceneBookmarkManager mgr;
    assert(mgr.Count() == 0);

    SceneBookmark bm;
    bm.name = "Start";
    bm.posX = 1.0f; bm.posY = 2.0f; bm.posZ = 3.0f;
    bm.yaw = 45.0f; bm.pitch = -10.0f;
    bm.zoom = 2.0f;
    uint32_t id = mgr.Save(bm);
    assert(id > 0);
    assert(mgr.Count() == 1);

    const auto* got = mgr.Get(id);
    assert(got != nullptr);
    assert(got->name == "Start");
    assert(got->posX == 1.0f);
    assert(got->zoom == 2.0f);

    std::cout << "[PASS] test_bookmark_save_get" << std::endl;
}

void test_bookmark_get_by_name() {
    SceneBookmarkManager mgr;
    SceneBookmark bm;
    bm.name = "Boss Arena";
    mgr.Save(bm);

    const auto* found = mgr.GetByName("Boss Arena");
    assert(found != nullptr);
    assert(found->name == "Boss Arena");

    assert(mgr.GetByName("Missing") == nullptr);

    std::cout << "[PASS] test_bookmark_get_by_name" << std::endl;
}

void test_bookmark_remove() {
    SceneBookmarkManager mgr;
    SceneBookmark bm;
    bm.name = "Temp";
    uint32_t id = mgr.Save(bm);
    assert(mgr.Count() == 1);

    assert(mgr.Remove(id));
    assert(mgr.Count() == 0);
    assert(mgr.Get(id) == nullptr);

    // Remove nonexistent
    assert(!mgr.Remove(999));

    std::cout << "[PASS] test_bookmark_remove" << std::endl;
}

void test_bookmark_all() {
    SceneBookmarkManager mgr;
    SceneBookmark a; a.name = "A";
    SceneBookmark b; b.name = "B";
    mgr.Save(a);
    mgr.Save(b);

    const auto& all = mgr.All();
    assert(all.size() == 2);
    assert(all[0].name == "A");
    assert(all[1].name == "B");

    std::cout << "[PASS] test_bookmark_all" << std::endl;
}

void test_bookmark_clear() {
    SceneBookmarkManager mgr;
    SceneBookmark bm; bm.name = "X";
    mgr.Save(bm);
    mgr.Save(bm);
    assert(mgr.Count() == 2);
    mgr.Clear();
    assert(mgr.Count() == 0);

    std::cout << "[PASS] test_bookmark_clear" << std::endl;
}

// ============================================================
// LayerTagSystem Tests
// ============================================================

void test_layer_create_and_query() {
    LayerTagSystem sys;
    assert(sys.CreateLayer("Foreground"));
    assert(sys.CreateLayer("Background"));
    assert(!sys.CreateLayer("Foreground")); // duplicate

    auto layers = sys.AllLayers();
    assert(layers.size() == 2);

    std::cout << "[PASS] test_layer_create_and_query" << std::endl;
}

void test_layer_visibility() {
    LayerTagSystem sys;
    sys.CreateLayer("UI");
    assert(sys.IsLayerVisible("UI"));

    sys.SetLayerVisible("UI", false);
    assert(!sys.IsLayerVisible("UI"));

    sys.SetLayerVisible("UI", true);
    assert(sys.IsLayerVisible("UI"));

    // Unknown layer defaults to visible
    assert(sys.IsLayerVisible("NonExistent"));

    std::cout << "[PASS] test_layer_visibility" << std::endl;
}

void test_layer_locked() {
    LayerTagSystem sys;
    sys.CreateLayer("Static");
    assert(!sys.IsLayerLocked("Static"));

    sys.SetLayerLocked("Static", true);
    assert(sys.IsLayerLocked("Static"));

    std::cout << "[PASS] test_layer_locked" << std::endl;
}

void test_layer_remove() {
    LayerTagSystem sys;
    sys.CreateLayer("Temp");
    sys.AssignLayer(1, "Temp");
    sys.AssignLayer(2, "Temp");

    assert(sys.RemoveLayer("Temp"));
    assert(sys.EntityLayer(1) == "");  // reassigned to default
    assert(sys.EntityLayer(2) == "");
    assert(!sys.RemoveLayer("Temp"));  // already removed

    std::cout << "[PASS] test_layer_remove" << std::endl;
}

void test_layer_assign_entity() {
    LayerTagSystem sys;
    sys.CreateLayer("World");
    sys.AssignLayer(10, "World");
    assert(sys.EntityLayer(10) == "World");

    auto entities = sys.EntitiesInLayer("World");
    assert(entities.size() == 1);
    assert(entities[0] == 10);

    // Unassigned entity
    assert(sys.EntityLayer(999) == "");

    std::cout << "[PASS] test_layer_assign_entity" << std::endl;
}

void test_layer_entity_visibility() {
    LayerTagSystem sys;
    sys.CreateLayer("Hidden");
    sys.SetLayerVisible("Hidden", false);
    sys.AssignLayer(5, "Hidden");

    assert(!sys.IsEntityVisible(5));

    sys.SetLayerVisible("Hidden", true);
    assert(sys.IsEntityVisible(5));

    // Entity without layer → visible
    assert(sys.IsEntityVisible(999));

    std::cout << "[PASS] test_layer_entity_visibility" << std::endl;
}

void test_tag_add_remove() {
    LayerTagSystem sys;
    sys.AddTag(1, "player");
    sys.AddTag(1, "controllable");
    assert(sys.HasTag(1, "player"));
    assert(sys.HasTag(1, "controllable"));
    assert(!sys.HasTag(1, "enemy"));

    sys.RemoveTag(1, "player");
    assert(!sys.HasTag(1, "player"));
    assert(sys.HasTag(1, "controllable"));

    std::cout << "[PASS] test_tag_add_remove" << std::endl;
}

void test_tag_entity_tags() {
    LayerTagSystem sys;
    sys.AddTag(1, "a");
    sys.AddTag(1, "b");
    sys.AddTag(1, "c");

    auto tags = sys.EntityTags(1);
    assert(tags.size() == 3);

    // Empty for unknown entity
    assert(sys.EntityTags(999).empty());

    std::cout << "[PASS] test_tag_entity_tags" << std::endl;
}

void test_tag_entities_with_tag() {
    LayerTagSystem sys;
    sys.AddTag(1, "enemy");
    sys.AddTag(2, "enemy");
    sys.AddTag(3, "player");

    auto enemies = sys.EntitiesWithTag("enemy");
    assert(enemies.size() == 2);

    auto players = sys.EntitiesWithTag("player");
    assert(players.size() == 1);

    assert(sys.EntitiesWithTag("npc").empty());

    std::cout << "[PASS] test_tag_entities_with_tag" << std::endl;
}

void test_layer_tag_remove_entity() {
    LayerTagSystem sys;
    sys.CreateLayer("World");
    sys.AssignLayer(1, "World");
    sys.AddTag(1, "active");

    sys.RemoveEntity(1);
    assert(sys.EntityLayer(1) == "");
    assert(!sys.HasTag(1, "active"));

    std::cout << "[PASS] test_layer_tag_remove_entity" << std::endl;
}

void test_layer_tag_clear() {
    LayerTagSystem sys;
    sys.CreateLayer("A");
    sys.AssignLayer(1, "A");
    sys.AddTag(1, "x");

    sys.Clear();
    assert(sys.AllLayers().empty());
    assert(sys.EntityLayer(1) == "");
    assert(!sys.HasTag(1, "x"));

    std::cout << "[PASS] test_layer_tag_clear" << std::endl;
}

void test_layer_auto_create() {
    LayerTagSystem sys;
    // AssignLayer should auto-create the layer
    sys.AssignLayer(1, "AutoCreated");
    assert(sys.GetLayer("AutoCreated") != nullptr);
    assert(sys.EntityLayer(1) == "AutoCreated");

    std::cout << "[PASS] test_layer_auto_create" << std::endl;
}
