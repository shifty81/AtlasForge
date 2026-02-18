#include <iostream>
#include <cassert>
#include <fstream>
#include <string>
#include <filesystem>
#include <cstdlib>

// Test the crash reporter tool and include firewall enhancements.

namespace fs = std::filesystem;

// ============================================================
// Crash Reporter Tests
// ============================================================

void test_crash_reporter_tool_exists() {
    // Verify crash_reporter.py exists
    std::string path = "tools/crash_reporter.py";
    if (!fs::exists(path)) path = "../tools/crash_reporter.py";
    assert(fs::exists(path));
    std::cout << "[PASS] test_crash_reporter_tool_exists" << std::endl;
}

void test_crash_reporter_help() {
    // Verify crash_reporter.py runs with --help
    std::string cmd = "python3 tools/crash_reporter.py --help > /dev/null 2>&1";
    int result = std::system(cmd.c_str());
    if (result != 0) {
        cmd = "python3 ../tools/crash_reporter.py --help > /dev/null 2>&1";
        result = std::system(cmd.c_str());
    }
    assert(result == 0);
    std::cout << "[PASS] test_crash_reporter_help" << std::endl;
}

void test_crash_reporter_validate_empty_dir() {
    // crash_reporter.py --validate-only on empty dir should fail (no manifests)
    std::string tmpDir = "/tmp/atlas_test_crash_empty";
    fs::create_directories(tmpDir);

    std::string cmd = "python3 tools/crash_reporter.py --dir " + tmpDir
                    + " --validate-only > /dev/null 2>&1";
    int result = std::system(cmd.c_str());
    if (result == -1) {
        cmd = "python3 ../tools/crash_reporter.py --dir " + tmpDir
            + " --validate-only > /dev/null 2>&1";
        result = std::system(cmd.c_str());
    }
    // Should return non-zero (no manifests found)
    assert(result != 0);

    fs::remove_all(tmpDir);
    std::cout << "[PASS] test_crash_reporter_validate_empty_dir" << std::endl;
}

void test_crash_reporter_validate_manifest() {
    // Create a mock .atlascrash manifest and validate it
    std::string tmpDir = "/tmp/atlas_test_crash_manifest";
    fs::create_directories(tmpDir);

    // Create mock save and replay files
    std::string savePath = tmpDir + "/desync_tick_100.asav";
    std::string replayPath = tmpDir + "/desync_tick_100.rply";
    std::string reportPath = tmpDir + "/desync_tick_100_report.txt";
    {
        std::ofstream(savePath) << "mock save data";
        std::ofstream(replayPath) << "mock replay data";
        std::ofstream(reportPath) << "mock report";
    }

    // Create manifest
    std::string manifestPath = tmpDir + "/desync_tick_100.atlascrash";
    {
        std::ofstream out(manifestPath);
        out << "atlas_crash_bundle_v1\n";
        out << "engine_version=dev\n";
        out << "platform=linux\n";
        out << "tick=100\n";
        out << "tick_rate=60\n";
        out << "seed=42\n";
        out << "local_hash=0xabc123\n";
        out << "remote_hash=0xdef456\n";
        out << "save=" << savePath << "\n";
        out << "replay=" << replayPath << "\n";
        out << "report=" << reportPath << "\n";
        out << "repro=./AtlasServer --repro --save \"" << savePath << "\"\n";
    }

    // Validate
    std::string cmd = "python3 tools/crash_reporter.py --dir " + tmpDir
                    + " --validate-only > /dev/null 2>&1";
    int result = std::system(cmd.c_str());
    if (result == -1) {
        cmd = "python3 ../tools/crash_reporter.py --dir " + tmpDir
            + " --validate-only > /dev/null 2>&1";
        result = std::system(cmd.c_str());
    }
    assert(result == 0);

    fs::remove_all(tmpDir);
    std::cout << "[PASS] test_crash_reporter_validate_manifest" << std::endl;
}

void test_crash_reporter_bundle() {
    // Create mock crash data and bundle it
    std::string tmpDir = "/tmp/atlas_test_crash_bundle";
    fs::create_directories(tmpDir);

    std::string savePath = tmpDir + "/desync_tick_200.asav";
    std::string replayPath = tmpDir + "/desync_tick_200.rply";
    std::string reportPath = tmpDir + "/desync_tick_200_report.txt";
    {
        std::ofstream(savePath) << "save data for tick 200";
        std::ofstream(replayPath) << "replay data for tick 200";
        std::ofstream(reportPath) << "report for tick 200";
    }

    std::string manifestPath = tmpDir + "/desync_tick_200.atlascrash";
    {
        std::ofstream out(manifestPath);
        out << "atlas_crash_bundle_v1\n";
        out << "engine_version=0.1.0\n";
        out << "platform=linux\n";
        out << "tick=200\n";
        out << "tick_rate=60\n";
        out << "seed=1234\n";
        out << "local_hash=0x111\n";
        out << "remote_hash=0x222\n";
        out << "save=" << savePath << "\n";
        out << "replay=" << replayPath << "\n";
        out << "report=" << reportPath << "\n";
        out << "repro=./AtlasServer --repro\n";
    }

    std::string outputPath = tmpDir + "/test_bundle.tar.gz";
    std::string cmd = "python3 tools/crash_reporter.py --dir " + tmpDir
                    + " --output " + outputPath + " > /dev/null 2>&1";
    int result = std::system(cmd.c_str());
    if (result == -1) {
        cmd = "python3 ../tools/crash_reporter.py --dir " + tmpDir
            + " --output " + outputPath + " > /dev/null 2>&1";
        result = std::system(cmd.c_str());
    }
    assert(result == 0);
    assert(fs::exists(outputPath));
    assert(fs::file_size(outputPath) > 0);

    fs::remove_all(tmpDir);
    std::cout << "[PASS] test_crash_reporter_bundle" << std::endl;
}

// ============================================================
// Include Firewall Enhancement Tests
// ============================================================

void test_include_firewall_header_exists() {
    std::string path = "engine/core/contract/IncludeFirewall.h";
    if (!fs::exists(path)) path = "../engine/core/contract/IncludeFirewall.h";
    assert(fs::exists(path));
    std::cout << "[PASS] test_include_firewall_header_exists" << std::endl;
}

void test_include_firewall_defines_marker() {
    // The IncludeFirewall.h should define ATLAS_INCLUDE_FIREWALL_ACTIVE
    std::string path = "engine/core/contract/IncludeFirewall.h";
    if (!fs::exists(path)) path = "../engine/core/contract/IncludeFirewall.h";

    std::ifstream file(path);
    assert(file.is_open());
    std::string content((std::istreambuf_iterator<char>(file)),
                         std::istreambuf_iterator<char>());
    assert(content.find("ATLAS_INCLUDE_FIREWALL_ACTIVE") != std::string::npos);
    std::cout << "[PASS] test_include_firewall_defines_marker" << std::endl;
}

void test_include_firewall_guards_simulation() {
    // Verify the firewall contains simulation layer guards
    std::string path = "engine/core/contract/IncludeFirewall.h";
    if (!fs::exists(path)) path = "../engine/core/contract/IncludeFirewall.h";

    std::ifstream file(path);
    assert(file.is_open());
    std::string content((std::istreambuf_iterator<char>(file)),
                         std::istreambuf_iterator<char>());
    assert(content.find("ATLAS_LAYER_SIMULATION") != std::string::npos);
    assert(content.find("GL_VERSION") != std::string::npos);
    assert(content.find("VULKAN_CORE_H_") != std::string::npos);
    assert(content.find("ATLAS_GL_RENDERER_H") != std::string::npos);
    assert(content.find("ATLAS_VULKAN_RENDERER_H") != std::string::npos);
    std::cout << "[PASS] test_include_firewall_guards_simulation" << std::endl;
}

void test_include_firewall_guards_core() {
    // Verify the firewall contains core layer guards
    std::string path = "engine/core/contract/IncludeFirewall.h";
    if (!fs::exists(path)) path = "../engine/core/contract/IncludeFirewall.h";

    std::ifstream file(path);
    assert(file.is_open());
    std::string content((std::istreambuf_iterator<char>(file)),
                         std::istreambuf_iterator<char>());
    assert(content.find("ATLAS_LAYER_CORE") != std::string::npos);
    assert(content.find("ATLAS_TICK_SCHEDULER_H") != std::string::npos);
    assert(content.find("ATLAS_REPLAY_RECORDER_H") != std::string::npos);
    std::cout << "[PASS] test_include_firewall_guards_core" << std::endl;
}

void test_include_firewall_ecs_no_render() {
    // Verify ecs/ source files don't include render headers
    std::string ecsDir = "engine/ecs";
    if (!fs::exists(ecsDir)) ecsDir = "../engine/ecs";
    if (!fs::exists(ecsDir)) {
        std::cout << "[PASS] test_include_firewall_ecs_no_render (skipped - dir not found)" << std::endl;
        return;
    }

    for (const auto& entry : fs::recursive_directory_iterator(ecsDir)) {
        if (!entry.is_regular_file()) continue;
        std::string ext = entry.path().extension().string();
        if (ext != ".h" && ext != ".cpp") continue;

        std::ifstream file(entry.path().string());
        std::string line;
        while (std::getline(file, line)) {
            auto includePos = line.find("#include");
            if (includePos == std::string::npos) continue;
            auto commentPos = line.find("//");
            if (commentPos != std::string::npos && commentPos < includePos) continue;
            assert(line.find("GLRenderer") == std::string::npos);
            assert(line.find("VulkanRenderer") == std::string::npos);
        }
    }

    std::cout << "[PASS] test_include_firewall_ecs_no_render" << std::endl;
}

void test_include_firewall_physics_no_render() {
    // Verify physics/ source files don't include render headers
    std::string physicsDir = "engine/physics";
    if (!fs::exists(physicsDir)) physicsDir = "../engine/physics";
    if (!fs::exists(physicsDir)) {
        std::cout << "[PASS] test_include_firewall_physics_no_render (skipped - dir not found)" << std::endl;
        return;
    }

    for (const auto& entry : fs::recursive_directory_iterator(physicsDir)) {
        if (!entry.is_regular_file()) continue;
        std::string ext = entry.path().extension().string();
        if (ext != ".h" && ext != ".cpp") continue;

        std::ifstream file(entry.path().string());
        std::string line;
        while (std::getline(file, line)) {
            auto includePos = line.find("#include");
            if (includePos == std::string::npos) continue;
            auto commentPos = line.find("//");
            if (commentPos != std::string::npos && commentPos < includePos) continue;
            assert(line.find("GLRenderer") == std::string::npos);
            assert(line.find("VulkanRenderer") == std::string::npos);
        }
    }

    std::cout << "[PASS] test_include_firewall_physics_no_render" << std::endl;
}

// ============================================================
// CI Workflow Tests
// ============================================================

void test_crash_reporter_workflow_exists() {
    std::string path = ".github/workflows/atlas_crash_reporter.yml";
    if (!fs::exists(path)) path = "../.github/workflows/atlas_crash_reporter.yml";
    assert(fs::exists(path));
    std::cout << "[PASS] test_crash_reporter_workflow_exists" << std::endl;
}
