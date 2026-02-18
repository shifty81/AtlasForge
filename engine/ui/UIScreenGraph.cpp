#include "UIScreenGraph.h"

namespace atlas::ui {

void UIScreen::Init(const std::string& name) {
    m_name = name;
    m_widgets.clear();
    m_nextId = 1;
}

const std::string& UIScreen::GetName() const {
    return m_name;
}

uint32_t UIScreen::AddWidget(UIWidgetType type, const std::string& name, float x, float y, float w, float h) {
    uint32_t id = m_nextId++;
    UIWidget widget;
    widget.id = id;
    widget.type = type;
    widget.name = name;
    widget.x = x;
    widget.y = y;
    widget.width = w;
    widget.height = h;
    m_widgets[id] = widget;
    return id;
}

void UIScreen::RemoveWidget(uint32_t id) {
    m_widgets.erase(id);
}

const UIWidget* UIScreen::GetWidget(uint32_t id) const {
    auto it = m_widgets.find(id);
    if (it != m_widgets.end()) {
        return &it->second;
    }
    return nullptr;
}

size_t UIScreen::WidgetCount() const {
    return m_widgets.size();
}

void UIScreen::SetVisible(uint32_t id, bool visible) {
    auto it = m_widgets.find(id);
    if (it != m_widgets.end()) {
        it->second.visible = visible;
    }
}

bool UIScreen::IsVisible(uint32_t id) const {
    auto it = m_widgets.find(id);
    if (it != m_widgets.end()) {
        return it->second.visible;
    }
    return false;
}

void UIScreen::SetParent(uint32_t childId, uint32_t parentId) {
    auto it = m_widgets.find(childId);
    if (it != m_widgets.end()) {
        it->second.parentId = parentId;
    }
}

std::vector<uint32_t> UIScreen::GetChildren(uint32_t parentId) const {
    std::vector<uint32_t> children;
    for (const auto& [id, widget] : m_widgets) {
        if (widget.parentId == parentId) {
            children.push_back(id);
        }
    }
    return children;
}

void UIScreen::SetMenuOpen(uint32_t id, bool open) {
    auto it = m_widgets.find(id);
    if (it != m_widgets.end()) {
        it->second.isMenuOpen = open;
    }
}

bool UIScreen::IsMenuOpen(uint32_t id) const {
    auto it = m_widgets.find(id);
    if (it != m_widgets.end()) {
        return it->second.isMenuOpen;
    }
    return false;
}

void UIScreen::SetHovered(uint32_t id, bool hovered) {
    auto it = m_widgets.find(id);
    if (it != m_widgets.end()) {
        it->second.isHovered = hovered;
    }
}

bool UIScreen::IsHovered(uint32_t id) const {
    auto it = m_widgets.find(id);
    if (it != m_widgets.end()) {
        return it->second.isHovered;
    }
    return false;
}

void UIScreen::SetSeparator(uint32_t id, bool isSeparator) {
    auto it = m_widgets.find(id);
    if (it != m_widgets.end()) {
        it->second.isSeparator = isSeparator;
    }
}

UIWidget* UIScreen::GetWidgetMutable(uint32_t id) {
    auto it = m_widgets.find(id);
    if (it != m_widgets.end()) {
        return &it->second;
    }
    return nullptr;
}

} // namespace atlas::ui
