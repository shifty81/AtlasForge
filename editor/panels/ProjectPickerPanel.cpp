#include "ProjectPickerPanel.h"
#include <filesystem>

namespace atlas::editor {

void ProjectPickerPanel::Draw() {
    // Stub: In a real implementation, this renders via Atlas UI
    // Modal dialog: "Open Project" button, recent project list, Exit button
    // File filter: *.atlas only
    // Once a project is selected, loads via ProjectManager
}

void ProjectPickerPanel::AddRecentProject(const std::string& path) {
    for (const auto& p : m_recentProjects) {
        if (p == path) return;
    }
    m_recentProjects.push_back(path);
}

bool ProjectPickerPanel::OpenProject(const std::string& atlasFilePath) {
    if (!std::filesystem::exists(atlasFilePath)) {
        return false;
    }
    if (!m_projectManager.Load(atlasFilePath)) {
        return false;
    }
    m_selectedPath = atlasFilePath;
    m_projectSelected = true;
    AddRecentProject(atlasFilePath);
    return true;
}

}
