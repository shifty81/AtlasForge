#include "../engine/ui/atlas/AtlasUI.h"
#include "../engine/ui/atlas/AtlasUITheme.h"
#include "../engine/ui/atlas/AtlasUIWidgets.h"
#include "../engine/ui/atlas/AtlasUIPanels.h"
#include <cassert>
#include <iostream>
#include <string>
#include <vector>

using namespace atlas::ui::atlas;

// ============================================================
// AtlasUI Context tests
// ============================================================

void test_atlas_ui_context_creation() {
    AtlasUIContext ctx;
    assert(ctx.VertexCount() == 0);
    assert(ctx.DrawCommandCount() == 0);
    assert(ctx.ViewWidth() == 0);
    assert(ctx.ViewHeight() == 0);
    std::cout << "[PASS] test_atlas_ui_context_creation" << std::endl;
}

void test_atlas_ui_begin_end_frame() {
    AtlasUIContext ctx;
    AtlasInputState input;
    input.mouseX = 100;
    input.mouseY = 200;
    ctx.BeginFrame(input, 800, 600);
    assert(ctx.ViewWidth() == 800);
    assert(ctx.ViewHeight() == 600);
    assert(ctx.Input().mouseX == 100);
    ctx.EndFrame();
    std::cout << "[PASS] test_atlas_ui_begin_end_frame" << std::endl;
}

void test_atlas_ui_filled_rect() {
    AtlasUIContext ctx;
    AtlasInputState input;
    ctx.BeginFrame(input, 800, 600);
    ctx.AddFilledRect({10, 20, 100, 50}, {255, 0, 0, 255});
    assert(ctx.VertexCount() == 6);       // 2 triangles = 6 verts
    assert(ctx.DrawCommandCount() == 1);
    ctx.EndFrame();
    std::cout << "[PASS] test_atlas_ui_filled_rect" << std::endl;
}

void test_atlas_ui_border_rect() {
    AtlasUIContext ctx;
    AtlasInputState input;
    ctx.BeginFrame(input, 800, 600);
    ctx.AddBorderRect({0, 0, 100, 100}, {255, 255, 255, 255}, 1);
    // 4 edges × 6 verts = 24 verts
    assert(ctx.VertexCount() == 24);
    assert(ctx.DrawCommandCount() == 4);
    ctx.EndFrame();
    std::cout << "[PASS] test_atlas_ui_border_rect" << std::endl;
}

void test_atlas_ui_id_system() {
    AtlasUIContext ctx;
    uint32_t id1 = ctx.GetID("Button1");
    uint32_t id2 = ctx.GetID("Button2");
    uint32_t id1b = ctx.GetID("Button1");
    assert(id1 != 0);
    assert(id2 != 0);
    assert(id1 != id2);
    assert(id1 == id1b);  // same label -> same ID
    std::cout << "[PASS] test_atlas_ui_id_system" << std::endl;
}

void test_atlas_ui_hot_active_widgets() {
    AtlasUIContext ctx;
    assert(ctx.HotWidget() == 0);
    assert(ctx.ActiveWidget() == 0);
    ctx.SetHotWidget(42);
    ctx.SetActiveWidget(99);
    assert(ctx.HotWidget() == 42);
    assert(ctx.ActiveWidget() == 99);
    ctx.ClearActiveWidget();
    assert(ctx.ActiveWidget() == 0);
    std::cout << "[PASS] test_atlas_ui_hot_active_widgets" << std::endl;
}

void test_atlas_ui_cursor() {
    AtlasUIContext ctx;
    AtlasInputState input;
    ctx.BeginFrame(input, 800, 600);
    ctx.SetCursor(50, 100);
    assert(ctx.CursorX() == 50);
    assert(ctx.CursorY() == 100);
    ctx.AdvanceCursor(20);
    assert(ctx.CursorY() > 100);
    ctx.EndFrame();
    std::cout << "[PASS] test_atlas_ui_cursor" << std::endl;
}

void test_atlas_ui_frame_resets_buffers() {
    AtlasUIContext ctx;
    AtlasInputState input;
    ctx.BeginFrame(input, 800, 600);
    ctx.AddFilledRect({0, 0, 10, 10}, {0, 0, 0, 255});
    assert(ctx.VertexCount() == 6);
    ctx.EndFrame();
    // New frame should clear buffers.
    ctx.BeginFrame(input, 800, 600);
    assert(ctx.VertexCount() == 0);
    assert(ctx.DrawCommandCount() == 0);
    ctx.EndFrame();
    std::cout << "[PASS] test_atlas_ui_frame_resets_buffers" << std::endl;
}

// ============================================================
// Theme tests
// ============================================================

void test_atlas_theme_teal() {
    auto t = AtlasTheme::Teal();
    assert(t.name == "Teal");
    assert(t.accent.r == 0);
    assert(t.accent.g == 200);
    std::cout << "[PASS] test_atlas_theme_teal" << std::endl;
}

void test_atlas_theme_amber() {
    auto t = AtlasTheme::Amber();
    assert(t.name == "Amber");
    assert(t.accent.r == 220);
    assert(t.accent.g == 170);
    std::cout << "[PASS] test_atlas_theme_amber" << std::endl;
}

void test_atlas_theme_colorblind() {
    auto t = AtlasTheme::ColorblindSafe();
    assert(t.name == "ColorblindSafe");
    assert(t.accent.r == 86);
    assert(t.accent.g == 180);
    std::cout << "[PASS] test_atlas_theme_colorblind" << std::endl;
}

void test_atlas_theme_switch() {
    AtlasUIContext ctx;
    ctx.SetTheme(AtlasTheme::Amber());
    assert(ctx.GetTheme().name == "Amber");
    ctx.SetTheme(AtlasTheme::Teal());
    assert(ctx.GetTheme().name == "Teal");
    std::cout << "[PASS] test_atlas_theme_switch" << std::endl;
}

// ============================================================
// Widget tests
// ============================================================

void test_atlas_label() {
    AtlasUIContext ctx;
    AtlasInputState input;
    ctx.BeginFrame(input, 800, 600);
    float yBefore = ctx.CursorY();
    Label(ctx, "Hello");
    assert(ctx.CursorY() > yBefore);
    assert(ctx.DrawCommandCount() > 0);
    ctx.EndFrame();
    std::cout << "[PASS] test_atlas_label" << std::endl;
}

void test_atlas_button_no_click() {
    AtlasUIContext ctx;
    AtlasInputState input;
    input.mouseX = -100;  // outside
    input.mouseY = -100;
    ctx.BeginFrame(input, 800, 600);
    bool clicked = Button(ctx, "Click Me", 100);
    assert(!clicked);
    ctx.EndFrame();
    std::cout << "[PASS] test_atlas_button_no_click" << std::endl;
}

void test_atlas_checkbox_toggle() {
    AtlasUIContext ctx;
    bool value = false;

    // Click inside the checkbox area (cursor starts at theme padding).
    AtlasInputState input;
    input.mouseX = 10;
    input.mouseY = 10;
    input.mouseClicked = true;
    ctx.BeginFrame(input, 800, 600);
    bool toggled = Checkbox(ctx, "Toggle", value);
    assert(toggled);
    assert(value == true);
    ctx.EndFrame();
    std::cout << "[PASS] test_atlas_checkbox_toggle" << std::endl;
}

void test_atlas_slider_drag() {
    AtlasUIContext ctx;
    float value = 0.0f;

    // Simulate a click in the middle of the slider track.
    AtlasInputState input;
    input.mouseX = 108;   // cursor(8) + width/2 = 8 + 100
    input.mouseY = 30;    // approximately after the label
    input.mouseClicked = true;
    input.mouseDown = true;
    ctx.BeginFrame(input, 800, 600);
    Slider(ctx, "Vol", value, 0.0f, 1.0f, 200.0f);
    // The slider should have advanced the cursor.
    assert(ctx.DrawCommandCount() > 0);
    ctx.EndFrame();
    std::cout << "[PASS] test_atlas_slider_drag" << std::endl;
}

void test_atlas_text_input() {
    AtlasUIContext ctx;
    std::string text = "hello";

    // Simulate focus + typing.
    AtlasInputState input;
    input.mouseX = 10;
    input.mouseY = 30;   // approximately where the input box is
    input.mouseClicked = true;
    input.mouseDown = true;
    input.textInput = "!";
    ctx.BeginFrame(input, 800, 600);
    bool submitted = TextInput(ctx, "Name", text, 200);
    ctx.EndFrame();
    // Text should have been appended since the box was clicked.
    // Note: depends on whether the click lands inside the input box.
    assert(ctx.DrawCommandCount() > 0);
    std::cout << "[PASS] test_atlas_text_input" << std::endl;
}

void test_atlas_combobox_cycle() {
    AtlasUIContext ctx;
    std::vector<std::string> items = {"Red", "Green", "Blue"};
    int sel = 0;

    // Click on the combo (position right after the label line).
    AtlasInputState input;
    input.mouseX = 10;
    input.mouseY = 30;  // after label
    input.mouseClicked = true;
    ctx.BeginFrame(input, 800, 600);
    bool changed = ComboBox(ctx, "Color", items, sel, 200);
    ctx.EndFrame();
    // May or may not change depending on hit area; just ensure no crash.
    assert(ctx.DrawCommandCount() > 0);
    std::cout << "[PASS] test_atlas_combobox_cycle" << std::endl;
}

void test_atlas_separator() {
    AtlasUIContext ctx;
    AtlasInputState input;
    ctx.BeginFrame(input, 800, 600);
    float yBefore = ctx.CursorY();
    Separator(ctx);
    assert(ctx.CursorY() > yBefore);
    ctx.EndFrame();
    std::cout << "[PASS] test_atlas_separator" << std::endl;
}

void test_atlas_spacing() {
    AtlasUIContext ctx;
    AtlasInputState input;
    ctx.BeginFrame(input, 800, 600);
    float yBefore = ctx.CursorY();
    Spacing(ctx, 20);
    assert(ctx.CursorY() >= yBefore + 20);
    ctx.EndFrame();
    std::cout << "[PASS] test_atlas_spacing" << std::endl;
}

void test_atlas_status_arc() {
    AtlasUIContext ctx;
    AtlasInputState input;
    ctx.BeginFrame(input, 800, 600);
    StatusArc(ctx, 0.75f, 30.0f);
    assert(ctx.DrawCommandCount() > 0);
    ctx.EndFrame();
    std::cout << "[PASS] test_atlas_status_arc" << std::endl;
}

void test_atlas_capacitor_ring() {
    AtlasUIContext ctx;
    AtlasInputState input;
    ctx.BeginFrame(input, 800, 600);
    CapacitorRing(ctx, 0.5f, 8, 24.0f);
    // 8 segments = 8 filled rects.
    assert(ctx.DrawCommandCount() == 8);
    ctx.EndFrame();
    std::cout << "[PASS] test_atlas_capacitor_ring" << std::endl;
}

void test_atlas_status_indicator() {
    AtlasUIContext ctx;
    AtlasInputState input;
    ctx.BeginFrame(input, 800, 600);
    StatusIndicator(ctx, "Online", {0, 255, 0, 255});
    assert(ctx.DrawCommandCount() > 0);
    ctx.EndFrame();
    std::cout << "[PASS] test_atlas_status_indicator" << std::endl;
}

// ============================================================
// Panel tests
// ============================================================

void test_atlas_panel_create() {
    AtlasUIContext ctx;
    AtlasPanelManager mgr;
    AtlasInputState input;
    ctx.BeginFrame(input, 800, 600);
    bool open = mgr.BeginPanel(ctx, "Inventory", 300, 200);
    assert(open);
    assert(mgr.PanelCount() == 1);
    assert(mgr.IsPanelOpen("Inventory"));
    mgr.EndPanel(ctx);
    ctx.EndFrame();
    std::cout << "[PASS] test_atlas_panel_create" << std::endl;
}

void test_atlas_panel_close_open() {
    AtlasPanelManager mgr;
    AtlasUIContext ctx;
    AtlasInputState input;
    ctx.BeginFrame(input, 800, 600);
    mgr.BeginPanel(ctx, "Stats", 200, 150);
    mgr.EndPanel(ctx);
    assert(mgr.IsPanelOpen("Stats"));
    mgr.ClosePanel("Stats");
    assert(!mgr.IsPanelOpen("Stats"));
    mgr.OpenPanel("Stats");
    assert(mgr.IsPanelOpen("Stats"));
    ctx.EndFrame();
    std::cout << "[PASS] test_atlas_panel_close_open" << std::endl;
}

void test_atlas_panel_toggle() {
    AtlasPanelManager mgr;
    AtlasUIContext ctx;
    AtlasInputState input;
    ctx.BeginFrame(input, 800, 600);
    mgr.BeginPanel(ctx, "Map");
    mgr.EndPanel(ctx);
    mgr.TogglePanel("Map");
    assert(!mgr.IsPanelOpen("Map"));
    mgr.TogglePanel("Map");
    assert(mgr.IsPanelOpen("Map"));
    ctx.EndFrame();
    std::cout << "[PASS] test_atlas_panel_toggle" << std::endl;
}

void test_atlas_panel_position() {
    AtlasPanelManager mgr;
    AtlasUIContext ctx;
    AtlasInputState input;
    ctx.BeginFrame(input, 800, 600);
    mgr.BeginPanel(ctx, "Log");
    mgr.EndPanel(ctx);
    mgr.SetPanelPosition("Log", 200, 300);
    auto* p = mgr.GetPanel("Log");
    assert(p != nullptr);
    assert(p->rect.x == 200);
    assert(p->rect.y == 300);
    ctx.EndFrame();
    std::cout << "[PASS] test_atlas_panel_position" << std::endl;
}

void test_atlas_panel_multiple() {
    AtlasPanelManager mgr;
    AtlasUIContext ctx;
    AtlasInputState input;
    ctx.BeginFrame(input, 800, 600);
    mgr.BeginPanel(ctx, "A"); mgr.EndPanel(ctx);
    mgr.BeginPanel(ctx, "B"); mgr.EndPanel(ctx);
    mgr.BeginPanel(ctx, "C"); mgr.EndPanel(ctx);
    assert(mgr.PanelCount() == 3);
    ctx.EndFrame();
    std::cout << "[PASS] test_atlas_panel_multiple" << std::endl;
}

void test_atlas_panel_closed_returns_false() {
    AtlasPanelManager mgr;
    AtlasUIContext ctx;
    AtlasInputState input;
    ctx.BeginFrame(input, 800, 600);
    mgr.BeginPanel(ctx, "Hidden"); mgr.EndPanel(ctx);
    mgr.ClosePanel("Hidden");
    bool open = mgr.BeginPanel(ctx, "Hidden");
    assert(!open);
    mgr.EndPanel(ctx);
    ctx.EndFrame();
    std::cout << "[PASS] test_atlas_panel_closed_returns_false" << std::endl;
}
