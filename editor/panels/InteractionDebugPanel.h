#pragma once
#include "../ui/EditorPanel.h"
#include "../../engine/interaction/Interaction.h"
#include <string>
#include <vector>

namespace atlas::editor {

struct InteractionLogEntry {
    std::string speaker;
    std::string text;
    std::string resolvedIntent;
    float confidence = 0.0f;
};

class InteractionDebugPanel : public EditorPanel {
public:
    const char* Name() const override { return "Interaction Debugger"; }
    void Draw() override;

    void LogInteraction(const InteractionLogEntry& entry);
    const std::vector<InteractionLogEntry>& GetLog() const { return m_log; }
    void ClearLog() { m_log.clear(); }
    size_t LogSize() const { return m_log.size(); }

private:
    std::vector<InteractionLogEntry> m_log;
};

}
