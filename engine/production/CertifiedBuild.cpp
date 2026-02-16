#include "CertifiedBuild.h"
#include <sstream>
#include <algorithm>

namespace atlas::production {

void CertifiedBuildSystem::SetLevel(CertificationLevel level) {
    m_level = level;
}

CertificationLevel CertifiedBuildSystem::GetLevel() const {
    return m_level;
}

void CertifiedBuildSystem::AddArtifact(const VerificationArtifact& artifact) {
    m_artifacts.push_back(artifact);
}

const VerificationArtifact* CertifiedBuildSystem::GetArtifact(const std::string& name) const {
    for (auto& a : m_artifacts) {
        if (a.name == name) return &a;
    }
    return nullptr;
}

std::vector<VerificationArtifact> CertifiedBuildSystem::ListArtifacts() const {
    return m_artifacts;
}

size_t CertifiedBuildSystem::ArtifactCount() const {
    return m_artifacts.size();
}

CertifiedBuildResult CertifiedBuildSystem::RunVerification() const {
    CertifiedBuildResult result;
    result.level = m_level;
    result.artifacts = m_artifacts;
    result.passed = true;
    for (auto& a : m_artifacts) {
        if (!a.passed) {
            result.passed = false;
            break;
        }
    }
    result.summary = result.passed ? "All verifications passed" : "Some verifications failed";
    return result;
}

std::string CertifiedBuildSystem::GenerateReport() const {
    std::ostringstream ss;
    ss << "Certified Build Report\n";
    ss << "Level: " << static_cast<int>(m_level) << "\n";
    ss << "Artifacts: " << m_artifacts.size() << "\n";
    for (auto& a : m_artifacts) {
        ss << "  " << a.name << " [" << a.type << "] - "
           << (a.passed ? "PASS" : "FAIL") << "\n";
    }
    return ss.str();
}

std::string CertifiedBuildSystem::ExportJSON() const {
    std::ostringstream ss;
    ss << "{\"level\":" << static_cast<int>(m_level)
       << ",\"artifacts\":[";
    bool first = true;
    for (auto& a : m_artifacts) {
        if (!first) ss << ",";
        first = false;
        ss << "{\"name\":\"" << a.name << "\""
           << ",\"type\":\"" << a.type << "\""
           << ",\"hash\":" << a.hash
           << ",\"passed\":" << (a.passed ? "true" : "false")
           << "}";
    }
    ss << "]}";
    return ss.str();
}

void CertifiedBuildSystem::Clear() {
    m_level = CertificationLevel::None;
    m_artifacts.clear();
}

} // namespace atlas::production
