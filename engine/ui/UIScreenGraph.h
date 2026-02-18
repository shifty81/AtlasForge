#pragma once
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace atlas::ui {

enum class UIWidgetType : uint8_t {
    Panel,
    Button,
    Text,
    Image,
    List,
    SlotGrid,
    InputField,
    Menu,
    MenuItem
};

struct UIWidget {
    uint32_t id = 0;
    UIWidgetType type = UIWidgetType::Panel;
    std::string name;
    bool visible = true;
    uint32_t parentId = 0;
    float x = 0.0f;
    float y = 0.0f;
    float width = 0.0f;
    float height = 0.0f;
    
    // Menu-specific state
    bool isMenuOpen = false;    // For Menu widgets: is dropdown shown?
    bool isHovered = false;     // For MenuItem widgets: is mouse over this item?
    bool isSeparator = false;   // For MenuItem widgets: is this a separator line?
};

class UIScreen {
public:
    void Init(const std::string& name);
    const std::string& GetName() const;

    uint32_t AddWidget(UIWidgetType type, const std::string& name, float x, float y, float w, float h);
    void RemoveWidget(uint32_t id);
    const UIWidget* GetWidget(uint32_t id) const;
    size_t WidgetCount() const;

    void SetVisible(uint32_t id, bool visible);
    bool IsVisible(uint32_t id) const;

    void SetParent(uint32_t childId, uint32_t parentId);
    std::vector<uint32_t> GetChildren(uint32_t parentId) const;

    // Menu state management
    void SetMenuOpen(uint32_t id, bool open);
    bool IsMenuOpen(uint32_t id) const;
    void SetHovered(uint32_t id, bool hovered);
    bool IsHovered(uint32_t id) const;
    void SetSeparator(uint32_t id, bool isSeparator);
    UIWidget* GetWidgetMutable(uint32_t id);

private:
    std::string m_name;
    std::unordered_map<uint32_t, UIWidget> m_widgets;
    uint32_t m_nextId = 1;
};

} // namespace atlas::ui
