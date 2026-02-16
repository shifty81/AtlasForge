#pragma once
#include <cstdint>
#include <string>
#include <vector>

namespace atlas::production {

enum class CertificationLevel : uint8_t {
    None,
    Basic,
    Standard,
    Full,
    AuditGrade
};

struct VerificationArtifact {
    std::string name;
    std::string type;
    uint64_t hash = 0;
    uint64_t timestamp = 0;
    bool passed = false;
    std::string details;
};

struct CertifiedBuildResult {
    CertificationLevel level = CertificationLevel::None;
    bool passed = false;
    std::vector<VerificationArtifact> artifacts;
    std::string summary;
    uint64_t timestamp = 0;
    std::string buildID;
};

class CertifiedBuildSystem {
public:
    void SetLevel(CertificationLevel level);
    CertificationLevel GetLevel() const;

    void AddArtifact(const VerificationArtifact& artifact);
    const VerificationArtifact* GetArtifact(const std::string& name) const;
    std::vector<VerificationArtifact> ListArtifacts() const;
    size_t ArtifactCount() const;

    CertifiedBuildResult RunVerification() const;
    std::string GenerateReport() const;
    std::string ExportJSON() const;

    void Clear();

private:
    CertificationLevel m_level = CertificationLevel::None;
    std::vector<VerificationArtifact> m_artifacts;
};

} // namespace atlas::production
