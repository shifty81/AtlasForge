#pragma once
#include <string>
#include <cstdint>

namespace atlas {

enum class RuntimeRole {
    Editor,
    Client,
    Server
};

enum class EngineMode {
    Editor,
    Client,
    Server
};

enum class Capability {
    AssetWrite,
    Rendering,
    Physics,
    GraphEdit,
    GraphExecute,
    NetAuthority,
    HotReload
};

struct EngineConfig {
    EngineMode mode = EngineMode::Client;
    std::string assetRoot = "assets";
    uint32_t tickRate = 30;
};

class Engine {
public:
    explicit Engine(const EngineConfig& cfg);
    ~Engine();

    void InitCore();
    void InitRender();
    void InitUI();
    void InitECS();
    void InitNetworking();
    void InitEditor();

    void Run();
    void RunEditor();
    void RunClient();
    void RunServer();

    bool Running() const;
    void Shutdown();

    bool Can(Capability cap) const;

    const EngineConfig& Config() const { return m_config; }

private:
    EngineConfig m_config;
    bool m_running = false;
};

}
