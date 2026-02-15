#include "UINodes.h"

namespace atlas::ui {

// --- PanelNode ---

std::vector<UIPort> PanelNode::Inputs() const {
    return {
        {"Width", UIPinType::Float},
        {"Height", UIPinType::Float}
    };
}

std::vector<UIPort> PanelNode::Outputs() const {
    return {{"Layout", UIPinType::Layout}};
}

void PanelNode::Evaluate(const UIContext& /*ctx*/, const std::vector<UIValue>& inputs, std::vector<UIValue>& outputs) const {
    float w = width;
    float h = height;
    if (!inputs.empty() && !inputs[0].data.empty()) {
        w = inputs[0].data[0];
    }
    if (inputs.size() > 1 && !inputs[1].data.empty()) {
        h = inputs[1].data[0];
    }

    outputs.resize(1);
    outputs[0].type = UIPinType::Layout;
    outputs[0].data = {0.0f, 0.0f, w, h};  // x, y, width, height
}

// --- ButtonNode ---

std::vector<UIPort> ButtonNode::Inputs() const {
    return {{"Layout", UIPinType::Layout}};
}

std::vector<UIPort> ButtonNode::Outputs() const {
    return {{"Action", UIPinType::Action}};
}

void ButtonNode::Evaluate(const UIContext& /*ctx*/, const std::vector<UIValue>& inputs, std::vector<UIValue>& outputs) const {
    outputs.resize(1);
    outputs[0].type = UIPinType::Action;
    // Action data: 1.0 if layout is valid (has data), 0.0 otherwise
    float active = 0.0f;
    if (!inputs.empty() && !inputs[0].data.empty()) {
        active = 1.0f;
    }
    outputs[0].data = {active};
}

// --- TextNode ---

std::vector<UIPort> TextNode::Inputs() const {
    return {
        {"Content", UIPinType::String},
        {"Layout", UIPinType::Layout}
    };
}

std::vector<UIPort> TextNode::Outputs() const {
    return {{"Layout", UIPinType::Layout}};
}

void TextNode::Evaluate(const UIContext& /*ctx*/, const std::vector<UIValue>& inputs, std::vector<UIValue>& outputs) const {
    std::string text = content;
    if (!inputs.empty() && !inputs[0].text.empty()) {
        text = inputs[0].text;
    }

    float x = 0.0f, y = 0.0f, w = 100.0f, h = 20.0f;
    if (inputs.size() > 1 && inputs[1].data.size() >= 4) {
        x = inputs[1].data[0];
        y = inputs[1].data[1];
        w = inputs[1].data[2];
        h = inputs[1].data[3];
    }

    outputs.resize(1);
    outputs[0].type = UIPinType::Layout;
    outputs[0].data = {x, y, w, h};
    outputs[0].text = text;
}

// --- ListNode ---

std::vector<UIPort> ListNode::Inputs() const {
    return {
        {"Layout", UIPinType::Layout},
        {"ItemCount", UIPinType::Float}
    };
}

std::vector<UIPort> ListNode::Outputs() const {
    return {{"Layout", UIPinType::Layout}};
}

void ListNode::Evaluate(const UIContext& /*ctx*/, const std::vector<UIValue>& inputs, std::vector<UIValue>& outputs) const {
    float x = 0.0f, y = 0.0f, w = 100.0f, h = 100.0f;
    if (!inputs.empty() && inputs[0].data.size() >= 4) {
        x = inputs[0].data[0];
        y = inputs[0].data[1];
        w = inputs[0].data[2];
        h = inputs[0].data[3];
    }

    float itemCount = 1.0f;
    if (inputs.size() > 1 && !inputs[1].data.empty()) {
        itemCount = inputs[1].data[0];
    }

    float totalHeight = h * itemCount;

    outputs.resize(1);
    outputs[0].type = UIPinType::Layout;
    outputs[0].data = {x, y, w, totalHeight};
}

}
