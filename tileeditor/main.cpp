///
/// AtlasTileEditor — standalone tile-editing tool.
///
/// Uses the same engine core, renderer, UI, and asset systems as the
/// full editor but launches directly into tile-editing mode.  This
/// allows artists and designers to work on tile maps without loading
/// the complete editor.
///
/// Usage:
///   AtlasTileEditor --project <path>
///

#include "../engine/core/Engine.h"
#include "../engine/core/Logger.h"
#include "../engine/ui/UIManager.h"
#include "../editor/tools/TileEditorModule.h"
#include "../editor/panels/TilePalettePanel.h"
#include "../editor/panels/RuleGraphEditorPanel.h"
#include <cstring>
#include <string>

static std::string ParseProjectArg(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], "--project") == 0 && i + 1 < argc) {
            return argv[i + 1];
        }
    }
    return {};
}

int main(int argc, char** argv) {
    atlas::core::Logger::Info("AtlasTileEditor starting");

    std::string projectPath = ParseProjectArg(argc, argv);
    if (projectPath.empty()) {
        atlas::core::Logger::Warn("No --project specified, using current directory");
        projectPath = ".";
    }

    // Initialise engine core (renderer, assets, UI)
    atlas::core::Engine engine;
    engine.Init(atlas::core::EngineMode::Editor);

    // Register tile editor module
    atlas::editor::TileEditorModule tileEditor;
    tileEditor.OnRegister();
    tileEditor.RegisterPanels();
    tileEditor.RegisterMenus();
    tileEditor.RegisterModes();

    // Create palette and graph panels
    atlas::editor::TilePalettePanel palette;
    atlas::editor::RuleGraphEditorPanel ruleGraph;

    atlas::core::Logger::Info("AtlasTileEditor ready — project: %s", projectPath.c_str());

    // Main loop would go here once the rendering backend is wired up.
    // For now the module and panels are initialised and can be
    // exercised through tests and headless mode.

    tileEditor.OnUnregister();
    engine.Shutdown();

    atlas::core::Logger::Info("AtlasTileEditor shut down");
    return 0;
}
