#include "InteractionDebugPanel.h"

namespace atlas::editor {

void InteractionDebugPanel::Draw() {
    // Stub: In a real implementation, this renders via Atlas UI
    // Shows: Speaker, Text, Intent, Confidence, Data Sources
    // Scrollable log of all interactions
    // Filter by speaker/intent type
}

void InteractionDebugPanel::LogInteraction(const InteractionLogEntry& entry) {
    m_log.push_back(entry);
}

}
