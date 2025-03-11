#include "UILabel.h"
#include "UICanvas.h"
#include <spdlog/spdlog.h>

namespace ui {

std::unique_ptr<UILabel> UILabel::create(const std::string* text) {
    return std::unique_ptr<UILabel>(new UILabel(text));
}

UILabel::UILabel(const std::string* text) {
    styleType_ = "label";
    if (text) setText(*text);
    setTextAlignment(TextAlignment::Center); // Default to Center for labels
    registerEventHandler("styleUpdate", [this](UIElement*, EventType) { onStyleUpdate(); });
}

void UILabel::setText(const std::string& text) {
    text_ = text;
    markDirty();
}

void UILabel::render(IRenderer* renderer) {
    if (!renderer || !dirty_) return;

    const UITheme* theme = nullptr;
    if (auto* canvas = dynamic_cast<UICanvas*>(parent_.value_or(nullptr))) {
        theme = canvas->getEffectiveTheme();
    }
    if (!theme) return;

    UIStyle style = theme->getStyle(styleType_);
    std::unordered_map<std::string, bool> states = {{"hovered", isHovered_}, {"pressed", isPressed_}};
    UIStyle effectiveStyle = style.computeEffectiveStyle(states);

    glm::vec2 textSize = renderer->measureText(text_, style.fontSize);
    glm::vec2 textPos = position_;

    // Apply anchor
    switch (anchor_) {
        case Anchor::TopLeft:
            textPos += glm::vec2(0.0f, 0.0f);
            break;
        case Anchor::TopRight:
            textPos.x += size_.x - textSize.x;
            break;
        case Anchor::BottomLeft:
            textPos.y += size_.y - textSize.y;
            break;
        case Anchor::BottomRight:
            textPos += size_ - textSize;
            break;
        case Anchor::Center:
            textPos += (size_ - textSize) / 2.0f;
            break;
    }

    // Adjust based on text alignment
    switch (getTextAlignment()) {
        case TextAlignment::Left:
            textPos.x = position_.x;
            break;
        case TextAlignment::Right:
            textPos.x = position_.x + size_.x - textSize.x;
            break;
        case TextAlignment::Center:
            textPos.x = position_.x + (size_.x - textSize.x) / 2.0f;
            break;
        default:
            textPos.x = position_.x + (size_.x - textSize.x) / 2.0f; // Default to Center
            break;
    }
    textPos.y += (size_.y + style.fontSize) / 2.0f - textSize.y;

    renderer->drawText(textPos, text_, effectiveStyle.textColor, style.fontSize);
    dirty_ = false;
}

} // namespace ui