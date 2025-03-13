#include "ui/UILabel.h"
#include "ui/UICanvas.h"
#include <spdlog/spdlog.h>
#include <glm/glm.hpp>

namespace ui {

    std::unique_ptr<UILabel> UILabel::create(const std::string& text) {
        return std::unique_ptr<UILabel>(new UILabel(text));
    }

    UILabel::UILabel(const std::string& text)
        : text_(text)
    {
        styleType_ = "label";
        setTextAlignment(TextAlignment::Center);
        registerEventHandler("styleUpdate", [this](UIElement*, EventType) { onStyleUpdate(); });
    }

    void UILabel::setText(const std::string& text) {
        text_ = text;
        markDirty();
    }

    std::string UILabel::getText() const {
        return text_;
    }

    void UILabel::render(IRenderer* renderer) {
        if (!renderer || !isDirty()) return;

        // Optionally draw background.
        drawBackground(renderer);

        const UITheme* theme = nullptr;
        if (auto* canvas = dynamic_cast<UICanvas*>(parent_.value_or(nullptr))) {
            theme = canvas->getEffectiveTheme();
        }
        if (!theme) return;

        UIStyle style = *theme->getStyle(styleType_).get();
        std::unordered_map<std::string, bool> states = { {"hovered", isHovered_}, {"pressed", isPressed_} };
        UIStyle effectiveStyle = style.computeEffectiveStyle(states);

        glm::vec2 textSize = renderer->measureText(text_, style.fontSize);
        glm::vec2 textPos = position_;

        // Position based on the anchor.
        switch (anchor_) {
        case Anchor::TopLeft:
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
            textPos += (size_ - textSize) * 0.5f;
            break;
        }
        // Adjust horizontally based on text alignment.
        switch (getTextAlignment()) {
        case TextAlignment::Left:
            textPos.x = position_.x;
            break;
        case TextAlignment::Right:
            textPos.x = position_.x + size_.x - textSize.x;
            break;
        case TextAlignment::Center:
            textPos.x = position_.x + (size_.x - textSize.x) * 0.5f;
            break;
        default:
            break;
        }
        // Vertical centering:
        textPos.y += (size_.y - textSize.y) * 0.5f;

        renderer->drawText(textPos, text_, effectiveStyle.textColor, style.fontSize);
        markDirty();
    }

    void UILabel::onStyleUpdate() {
        markDirty();
    }

} // namespace ui
