#include "ConsolePanel.h"

namespace atlas::editor {

void ConsolePanel::Draw() {
    // Stub: In a real implementation, this would render via Atlas UI
    // Display history, input field, execute button
}

void ConsolePanel::AddLine(const std::string& line) {
    m_history.push_back(line);
}

void ConsolePanel::Execute(const std::string& command) {
    m_history.push_back("> " + command);
    // Stub: parse and execute command
    // Examples: spawn_entity, set tickrate 30, net mode p2p, ecs.dump
}

}
