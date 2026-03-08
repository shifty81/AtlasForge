#include "DependencyGraph.h"

namespace atlas::asset {

void DependencyGraph::AddDependency(const std::string& assetId,
                                     const std::string& dependsOn) {
    for (const auto& dep : m_dependencies) {
        if (dep.assetId == assetId && dep.dependsOn == dependsOn) return;
    }
    m_dependencies.push_back({assetId, dependsOn});
}

void DependencyGraph::RemoveDependency(const std::string& assetId,
                                        const std::string& dependsOn) {
    for (auto it = m_dependencies.begin(); it != m_dependencies.end(); ++it) {
        if (it->assetId == assetId && it->dependsOn == dependsOn) {
            m_dependencies.erase(it);
            return;
        }
    }
}

std::vector<std::string> DependencyGraph::GetDependencies(
        const std::string& assetId) const {
    std::vector<std::string> result;
    for (const auto& dep : m_dependencies) {
        if (dep.assetId == assetId) {
            result.push_back(dep.dependsOn);
        }
    }
    return result;
}

std::vector<std::string> DependencyGraph::GetDependents(
        const std::string& assetId) const {
    std::vector<std::string> result;
    for (const auto& dep : m_dependencies) {
        if (dep.dependsOn == assetId) {
            result.push_back(dep.assetId);
        }
    }
    return result;
}

bool DependencyGraph::HasCircularDependency(const std::string& assetId) const {
    // DFS with recursion stack to detect back edges.
    std::set<std::string> visited;
    std::set<std::string> inStack;
    std::stack<std::pair<std::string, bool>> stack;
    stack.push({assetId, false});

    while (!stack.empty()) {
        auto& [current, processed] = stack.top();

        if (processed) {
            inStack.erase(current);
            stack.pop();
            continue;
        }

        processed = true;

        if (visited.count(current)) {
            stack.pop();
            continue;
        }

        visited.insert(current);
        inStack.insert(current);

        for (const auto& dep : m_dependencies) {
            if (dep.assetId == current) {
                if (inStack.count(dep.dependsOn)) return true;
                if (!visited.count(dep.dependsOn)) {
                    stack.push({dep.dependsOn, false});
                }
            }
        }
    }
    return false;
}

std::vector<std::string> DependencyGraph::ResolveBuildOrder() const {
    // Collect all nodes
    std::set<std::string> allNodes;
    for (const auto& dep : m_dependencies) {
        allNodes.insert(dep.assetId);
        allNodes.insert(dep.dependsOn);
    }

    // Build adjacency and in-degree
    std::unordered_map<std::string, std::vector<std::string>> adj;
    std::unordered_map<std::string, int> inDegree;
    for (const auto& node : allNodes) {
        inDegree[node] = 0;
    }
    for (const auto& dep : m_dependencies) {
        adj[dep.dependsOn].push_back(dep.assetId);
        inDegree[dep.assetId]++;
    }

    // Kahn's algorithm
    std::deque<std::string> queue;
    for (const auto& [node, deg] : inDegree) {
        if (deg == 0) queue.push_back(node);
    }
    std::sort(queue.begin(), queue.end());

    std::vector<std::string> result;
    while (!queue.empty()) {
        std::string current = queue.front();
        queue.pop_front();
        result.push_back(current);

        if (adj.count(current)) {
            std::vector<std::string> next;
            for (const auto& neighbor : adj[current]) {
                inDegree[neighbor]--;
                if (inDegree[neighbor] == 0) {
                    next.push_back(neighbor);
                }
            }
            std::sort(next.begin(), next.end());
            for (const auto& n : next) {
                queue.push_back(n);
            }
        }
    }
    return result;
}

void DependencyGraph::Clear() {
    m_dependencies.clear();
}

}  // namespace atlas::asset
