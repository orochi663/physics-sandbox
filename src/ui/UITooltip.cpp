#include "ui/UITooltip.h"
#include "ui/UITheme.h"
#include "ui/UIStyle.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace ui {

UITooltip::UITooltip(const std::string& text)
    : text_(text), delay_(0.5f) // default delay 0.5 seconds
{}

void UITooltip::setText(const std::string& text) {
    text_ = text;
}

std::string UITooltip::getText() const {
    return text_;
}

void UITooltip::setDelay(float delay) {
    delay_ = delay;
}

float UITooltip::getDelay() const {
    return delay_;
}

void UITooltip::render(IRenderer* renderer, const glm::vec2& parentPos, const glm::vec2& parentSize) {
    if (!renderer) return;

    // Retrieve the tooltip style from the effective theme.
    // We assume that the UITheme for tooltips is stored under the key "tooltip".
    // In a full implementation, the UITooltip might be provided its theme reference.
    const UITheme* theme = nullptr;
    // For this example, we assume a global theme exists (this should be set by your UIManager)
    // Here, we use a simple fallback.
    // In practice, you would pass the theme to UITooltip or retrieve it from UIManager.
    // For now, we create a default style:
    UIStyle tooltipStyle;
    // Override tooltip-specific properties:
    tooltipStyle.backgroundColor = glm::vec4(0.0f, 0.0f, 0.0f, 0.75f); // semi-transparent black
    // Use our tooltip members:
    tooltipStyle.tooltipFadeInTime = 0.3f;
    tooltipStyle.tooltipFadeOutTime = 0.3f;
    tooltipStyle.tooltipShape = TooltipShape::Balloon; // example: balloon shape
    tooltipStyle.tooltipPointerSize = 10.0f;
    tooltipStyle.tooltipPointerOffset = 5.0f;

    // For this example, we'll use these defaults.
    // Compute text size.
    float fontSize = tooltipStyle.fontSize > 0.0f ? tooltipStyle.fontSize : 14.0f;
    glm::vec2 textSize = renderer->measureText(text_, fontSize);
    float padding = 5.0f;
    glm::vec2 tooltipSize = glm::vec2(textSize.x + 2 * padding, textSize.y + 2 * padding);

    // For a balloon shape, you might adjust the shape here (this example uses a simple rectangle).
    // Position: center horizontally above the parent element, with a gap.
    glm::vec2 tooltipPos = glm::vec2(
        parentPos.x + (parentSize.x - tooltipSize.x) * 0.5f,
        parentPos.y - tooltipSize.y - 5.0f
    );

    // Render background: if a background texture is set in tooltipStyle, use it; otherwise, a solid fill.
    if (tooltipStyle.backgroundTexture) {
        renderer->drawTexture(tooltipPos, tooltipSize, tooltipStyle.backgroundTexture.get());
    } else {
        renderer->drawRect(tooltipPos, tooltipSize, tooltipStyle.backgroundColor);
    }
    // Render the tooltip text within the tooltip rectangle.
    glm::vec2 textPos = tooltipPos + glm::vec2(padding, tooltipSize.y - padding - textSize.y);
    renderer->drawText(textPos, text_, tooltipStyle.textColor, fontSize);
}

} // namespace ui
