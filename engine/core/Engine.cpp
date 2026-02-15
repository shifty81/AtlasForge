#include "Engine.h"
#include "Logger.h"

namespace atlas {

Engine::Engine(const EngineConfig& cfg) : m_config(cfg) {
}

Engine::~Engine() {
    Shutdown();
}

void Engine::InitCore() {
    Logger::Init();
    Logger::Info("Engine core initialized");
    m_running = true;
}

void Engine::InitRender() {
    if (m_config.mode == EngineMode::Server) {
        Logger::Info("Server mode: rendering disabled");
        return;
    }
    Logger::Info("Render system initialized");
}

void Engine::InitUI() {
    if (m_config.mode == EngineMode::Server) {
        Logger::Info("Server mode: UI disabled");
        return;
    }
    Logger::Info("UI system initialized");
}

void Engine::InitECS() {
    Logger::Info("ECS initialized (empty world)");
}

void Engine::InitNetworking() {
    Logger::Info("Networking initialized (idle)");
}

void Engine::InitEditor() {
    if (m_config.mode != EngineMode::Editor) {
        return;
    }
    Logger::Info("Editor tools initialized");
}

void Engine::Run() {
    switch (m_config.mode) {
        case EngineMode::Editor: RunEditor(); break;
        case EngineMode::Client: RunClient(); break;
        case EngineMode::Server: RunServer(); break;
    }
}

void Engine::RunEditor() {
    Logger::Info("Running Atlas Editor");
    // Editor main loop stub
    // In a real implementation, this would:
    // 1. Poll input
    // 2. Poll hot reload
    // 3. Update UI
    // 4. Render
}

void Engine::RunClient() {
    Logger::Info("Running Atlas Client");
    // Client main loop stub
}

void Engine::RunServer() {
    Logger::Info("Running Atlas Server");
    // Server main loop stub: tick-based simulation
    // const float tickRate = 1.0f / m_config.tickRate;
    // while (m_running) {
    //     net.Poll();
    //     ecs.Update(tickRate);
    //     graphVM.Tick();
    //     net.Flush();
    //     SleepUntil(start + tickRate);
    // }
}

bool Engine::Running() const {
    return m_running;
}

void Engine::Shutdown() {
    if (m_running) {
        Logger::Info("Engine shutting down");
        m_running = false;
    }
}

bool Engine::Can(Capability cap) const {
    switch (cap) {
        case Capability::AssetWrite:
            return m_config.mode == EngineMode::Editor;
        case Capability::Rendering:
            return m_config.mode != EngineMode::Server;
        case Capability::Physics:
            return true;
        case Capability::GraphEdit:
            return m_config.mode == EngineMode::Editor;
        case Capability::GraphExecute:
            return true;
        case Capability::NetAuthority:
            return m_config.mode == EngineMode::Server;
        case Capability::HotReload:
            return m_config.mode == EngineMode::Editor;
    }
    return false;
}

}
