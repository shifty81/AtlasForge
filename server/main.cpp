#include "core/Engine.h"
#include <iostream>

int main() {
    atlas::EngineConfig cfg;
    cfg.mode = atlas::EngineMode::Server;

    atlas::Engine engine(cfg);
    engine.InitCore();
    engine.InitECS();
    engine.InitNetworking();

    engine.Run();

    return 0;
}
