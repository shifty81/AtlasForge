#pragma once
#include "UIScreenGraph.h"
#include "UIEventRouter.h"
#include <cstdint>
#include <functional>

namespace atlas::ui {

/// Callback invoked when a menu item is selected.
/// Parameters: menuId, menuItemId
using MenuItemCallback = std::function<void(uint32_t, uint32_t)>;

/// Manages menu state and interactions for dropdown menus.
/// Handles menu button clicks, dropdown visibility, menu item selection,
/// and click-away behavior.
class MenuManager {
public:
    /// Initialize with a reference to the UI screen.
    void Init(UIScreen* screen);

    /// Update menu state based on input events.
    /// Call this before rendering to update hover states and handle clicks.
    /// Returns true if the event was consumed by the menu system.
    bool HandleEvent(const UIEvent& event);

    /// Update menu states (hover, etc) based on mouse position.
    void Update(int32_t mouseX, int32_t mouseY);

    /// Close all open menus.
    void CloseAllMenus();

    /// Set a callback to be invoked when a menu item is clicked.
    void SetMenuItemCallback(MenuItemCallback callback);

    /// Check if a point is inside a widget's bounds.
    bool IsPointInWidget(const UIWidget* widget, int32_t x, int32_t y) const;

private:
    UIScreen* m_screen = nullptr;
    uint32_t m_openMenuId = 0;  // ID of currently open menu (0 = none)
    MenuItemCallback m_menuItemCallback;
};

} // namespace atlas::ui
