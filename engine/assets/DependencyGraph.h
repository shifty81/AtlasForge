#pragma once
// ============================================================
// Atlas DependencyGraph — Shared Dependency Tracking Utility
// ============================================================
//
// Provides a directed graph for tracking asset dependencies with
// cycle detection and topological ordering.  Used by both
// AssetRegistry and AssetValidator to avoid code duplication.

#include <string>
#include <vector>
#include <set>
#include <stack>
#include <deque>
#include <unordered_map>
#include <algorithm>

namespace atlas::asset {

/// A directed edge in the asset dependency graph.
struct AssetDependency {
    std::string assetId;
    std::string dependsOn;
};

/// Reusable directed dependency graph with cycle detection and
/// topological sort (Kahn's algorithm).
class DependencyGraph {
public:
    void AddDependency(const std::string& assetId, const std::string& dependsOn);
    void RemoveDependency(const std::string& assetId, const std::string& dependsOn);

    std::vector<std::string> GetDependencies(const std::string& assetId) const;
    std::vector<std::string> GetDependents(const std::string& assetId) const;

    bool HasCircularDependency(const std::string& assetId) const;
    std::vector<std::string> ResolveBuildOrder() const;

    std::vector<AssetDependency> AllDependencies() const { return m_dependencies; }
    size_t Count() const { return m_dependencies.size(); }
    void Clear();

private:
    std::vector<AssetDependency> m_dependencies;
};

}  // namespace atlas::asset
