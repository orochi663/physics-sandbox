#include "UIStyle.h"

namespace ui {

UIStyle::UIStyle() {
    stateStyles["hovered"] = StateStyle{glm::vec4(0.7f, 0.7f, 0.7f, 1.0f)};
    stateStyles["pressed"] = StateStyle{glm::vec4(0.3f, 0.3f, 0.3f, 1.0f)};
}

void UIStyle::merge(const UIStyle& other) {
    if (other.backgroundTexture) backgroundTexture = other.backgroundTexture;
    if (other.backgroundColor != glm::vec4(0.0f)) backgroundColor = other.backgroundColor;
    if (other.textColor != glm::vec4(0.0f)) textColor = other.textColor;
    if (other.fontSize > 0.0f) fontSize = other.fontSize;
    if (!other.fontName.empty()) fontName = other.fontName;
    if (other.separatorColor != glm::vec4(0.0f)) separatorColor = other.separatorColor;
    if (other.useBeautification) useBeautification = other.useBeautification;

    for (const auto& [state, style] : other.stateStyles) {
        stateStyles[state] = style;
    }
}

UIStyle UIStyle::computeEffectiveStyle(const std::unordered_map<std::string, bool>& states) const {
    UIStyle effective = *this;
    for (const auto& [state, active] : states) {
        auto it = stateStyles.find(state);
        if (active && it != stateStyles.end()) {
            if (it->second.backgroundColor != glm::vec4(0.0f)) {
                effective.backgroundColor = it->second.backgroundColor;
            }
            if (it->second.backgroundTexture) {
                effective.backgroundTexture = it->second.backgroundTexture;
            }
            if (it->second.separatorColor != glm::vec4(0.0f)) {
                effective.separatorColor = it->second.separatorColor;
            }
            if (it->second.useBeautification) {
                effective.useBeautification = it->second.useBeautification;
            }
        }
    }
    return effective;
}

} // namespace ui