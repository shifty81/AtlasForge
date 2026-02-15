#pragma once
#include "../ui/EditorPanel.h"
#include <string>
#include <vector>

namespace atlas::editor {

class ConsolePanel : public EditorPanel {
public:
    const char* Name() const override { return "Console"; }
    void Draw() override;

    void AddLine(const std::string& line);
    void Execute(const std::string& command);

private:
    std::vector<std::string> m_history;
    std::string m_inputBuffer;
};

}
