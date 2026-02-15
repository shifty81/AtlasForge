#include "core/Engine.h"
#include <iostream>

int main() {
    atlas::EngineConfig cfg;
    cfg.mode = atlas::EngineMode::Client;

    atlas::Engine engine(cfg);
    engine.InitCore();
    engine.InitRender();
    engine.InitUI();
    engine.InitECS();
    engine.InitNetworking();

    engine.Run();

    return 0;
}
