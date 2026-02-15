#pragma once
#include "../ui/EditorPanel.h"
#include "../../engine/world/WorldGraph.h"
#include <string>

namespace atlas::editor {

class WorldGraphPanel : public EditorPanel {
public:
    const char* Name() const override { return "World Graph Editor"; }
    void Draw() override;

    void SetGraph(world::WorldGraph* graph) { m_graph = graph; }
    world::WorldGraph* GetGraph() { return m_graph; }

    bool CompileGraph();
    bool ExecutePreview(const world::WorldGenContext& ctx);

    void SelectNode(world::NodeID id) { m_selectedNode = id; }
    world::NodeID SelectedNode() const { return m_selectedNode; }

private:
    world::WorldGraph* m_graph = nullptr;
    world::NodeID m_selectedNode = 0;
};

}
