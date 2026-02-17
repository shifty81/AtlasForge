#pragma once
#include "../ui/EditorPanel.h"
#include "../tools/PlayInEditor.h"
#include "ReplayTimelinePanel.h"
#include "StateHashDiffPanel.h"
#include "JobTracePanel.h"
#include <string>
#include <sstream>

namespace atlas::editor {

/// Combined "Truth UI" dashboard panel.
/// Aggregates simulation control, state hash inspection, replay timeline,
/// and job execution trace into a single debugger view.
class TruthUIPanel : public EditorPanel {
public:
    const char* Name() const override { return "Truth UI"; }

    void Draw() override {
        // Section 1: Simulation Control
        //   Tick step, pause/resume, current tick, PIE mode indicator.
        // Section 2: Determinism View
        //   Current tick hash, first divergence tick, platform signature.
        // Section 3: Replay Timeline
        //   Timeline scrubber, markers, divergence highlight.
        // Section 4: Job Trace
        //   System execution order, order mismatch detection.
    }

    // --- Wiring ---

    void SetPlayInEditor(PlayInEditor* pie) { m_pie = pie; }
    PlayInEditor* GetPlayInEditor() const { return m_pie; }

    void SetReplayTimeline(ReplayTimelinePanel* timeline) { m_timeline = timeline; }
    ReplayTimelinePanel* GetReplayTimeline() const { return m_timeline; }

    void SetStateHashDiff(StateHashDiffPanel* hashDiff) { m_hashDiff = hashDiff; }
    StateHashDiffPanel* GetStateHashDiff() const { return m_hashDiff; }

    void SetJobTrace(JobTracePanel* jobTrace) { m_jobTrace = jobTrace; }
    JobTracePanel* GetJobTrace() const { return m_jobTrace; }

    // --- Status queries ---

    bool IsSimulating() const {
        if (!m_pie) return false;
        auto mode = m_pie->Mode();
        return mode == PIEMode::Simulating || mode == PIEMode::Possessed;
    }

    bool IsPaused() const {
        if (!m_pie) return false;
        return m_pie->Mode() == PIEMode::Paused;
    }

    bool HasDivergence() const {
        if (m_hashDiff && m_hashDiff->HasDivergence()) return true;
        if (m_timeline && m_timeline->HasDivergence()) return true;
        return false;
    }

    bool HasOrderMismatch() const {
        if (!m_jobTrace) return false;
        return m_jobTrace->HasOrderMismatch();
    }

    std::string Summary() const {
        std::ostringstream oss;
        oss << "Truth UI";

        if (m_pie) {
            switch (m_pie->Mode()) {
                case PIEMode::Stopped:    oss << " [Stopped]"; break;
                case PIEMode::Simulating: oss << " [Simulating]"; break;
                case PIEMode::Paused:     oss << " [Paused]"; break;
                case PIEMode::Possessed:  oss << " [Possessed]"; break;
            }
            oss << " Ticks: " << m_pie->TicksSimulated();
        }

        if (HasDivergence()) oss << " ⚠ DIVERGENCE";
        if (HasOrderMismatch()) oss << " ⚠ ORDER MISMATCH";

        return oss.str();
    }

private:
    PlayInEditor* m_pie = nullptr;
    ReplayTimelinePanel* m_timeline = nullptr;
    StateHashDiffPanel* m_hashDiff = nullptr;
    JobTracePanel* m_jobTrace = nullptr;
};

} // namespace atlas::editor
