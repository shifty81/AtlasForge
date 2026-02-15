#include "VoiceCommandPanel.h"

namespace atlas::editor {

void VoiceCommandPanel::Draw() {
    // Stub: In a real implementation, this renders via Atlas UI
    // Shows: listening status, last phrase, resolved intent, confidence
    // Buttons: [Enable] [Disable] [Train]
    // List of registered voice commands
}

void VoiceCommandPanel::ProcessPhrase(const std::string& phrase) {
    m_lastPhrase = phrase;
    m_lastIntent.clear();

    auto* cmd = m_voiceRegistry.Match(phrase);
    if (cmd) {
        m_lastIntent = cmd->intentName;
        interaction::Intent intent;
        intent.name = cmd->intentName;
        intent.source = interaction::IntentSource::Voice;
        m_intentRegistry.Dispatch(intent);
    }
}

}
