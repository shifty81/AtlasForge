#include "GameMechanicsUIPanel.h"
#include <sstream>
#include <algorithm>

namespace atlas::editor {

void GameMechanicsUIPanel::Draw() {
    // UI rendering handled by backend.
}

uint64_t GameMechanicsUIPanel::AddElement(const MechanicsUIElement& element) {
    MechanicsUIElement e = element;
    e.id = m_nextID++;
    m_elements.push_back(e);
    return e.id;
}

bool GameMechanicsUIPanel::RemoveElement(uint64_t id) {
    auto it = std::find_if(m_elements.begin(), m_elements.end(),
                           [id](const MechanicsUIElement& e) { return e.id == id; });
    if (it != m_elements.end()) {
        if (m_selectedID == id) m_selectedID = 0;
        m_elements.erase(it);
        return true;
    }
    return false;
}

const MechanicsUIElement* GameMechanicsUIPanel::GetElement(uint64_t id) const {
    for (auto& e : m_elements) {
        if (e.id == id) return &e;
    }
    return nullptr;
}

std::vector<MechanicsUIElement> GameMechanicsUIPanel::ListElements() const {
    return m_elements;
}

size_t GameMechanicsUIPanel::ElementCount() const {
    return m_elements.size();
}

void GameMechanicsUIPanel::SelectElement(uint64_t id) {
    m_selectedID = id;
}

uint64_t GameMechanicsUIPanel::SelectedElementID() const {
    return m_selectedID;
}

void GameMechanicsUIPanel::SetPreviewMode(bool enabled) {
    m_previewMode = enabled;
}

bool GameMechanicsUIPanel::IsPreviewMode() const {
    return m_previewMode;
}

std::string GameMechanicsUIPanel::ExportLayout() const {
    std::ostringstream ss;
    ss << "{\"elements\":[";
    bool first = true;
    for (auto& e : m_elements) {
        if (!first) ss << ",";
        first = false;
        ss << "{\"id\":" << e.id
           << ",\"name\":\"" << e.name << "\""
           << ",\"type\":\"" << e.elementType << "\"}";
    }
    ss << "]}";
    return ss.str();
}

bool GameMechanicsUIPanel::ImportLayout(const std::string& json) {
    if (json.empty()) return false;
    if (json.find("\"elements\"") == std::string::npos) return false;
    return true;
}

void GameMechanicsUIPanel::Clear() {
    m_elements.clear();
    m_nextID = 1;
    m_selectedID = 0;
    m_previewMode = false;
}

} // namespace atlas::editor
