#include "WorldGraphPanel.h"

namespace atlas::editor {

void WorldGraphPanel::Draw() {
    // Stub: In a real implementation, this renders via Atlas UI
    // Left panel: Node Palette (categories: Input, Noise, Math, Fields, Utilities)
    // Center: Graph Canvas (infinite 2D, pan/zoom, nodes + links)
    // Bottom: Node Inspector, Chunk Preview, Profiler
    // Nodes show: title, properties, input/output pins
    // Links: bezier curves color-coded by pin type
}

bool WorldGraphPanel::CompileGraph() {
    if (!m_graph) return false;
    return m_graph->Compile();
}

bool WorldGraphPanel::ExecutePreview(const world::WorldGenContext& ctx) {
    if (!m_graph || !m_graph->IsCompiled()) return false;
    return m_graph->Execute(ctx);
}

}
