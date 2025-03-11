#include "UIButton.h"
#include "UILabel.h"
#include "UICanvas.h"
#include <spdlog/spdlog.h>

namespace ui {

std::unique_ptr<UIButton> UIButton::create(const std::string& labelText) {
    return std::unique_ptr<UIButton>(new UIButton(labelText));
}

UIButton::UIButton(const std::string& labelText) {
    styleType_ = "button";
    label_ = std::make_unique<UILabel>(this);
    label_->setText(labelText);
    label_->setStyleType("buttonLabel");
    setTextAlignment(TextAlignment::Center); // Default to Center for symmetry
    registerEventHandler("styleUpdate", [this](UIElement*, EventType) { onStyleUpdate(); });
    setOnClick([]() { spdlog::info("Button clicked!"); }); // Default callback
}

void UIButton::render(IRenderer* renderer) {
    if (!renderer || !dirty_) return;

    const UITheme* theme = nullptr;
    if (auto* canvas = dynamic_cast<UICanvas*>(parent_.value_or(nullptr))) {
        theme = canvas->getEffectiveTheme();
    }
    if (!theme) return;

    UIStyle style = theme->getStyle(styleType_);
    std::unordered_map<std::string, bool> states = {{"hovered", isHovered_}, {"pressed", isPressed_}};
    UIStyle effectiveStyle = style.computeEffectiveStyle(states);

    glm::vec2 adjustedPos = getPosition();
    if (isPressed_) {
        adjustedPos += glm::vec2(2.0f, 2.0f);
    }

    if (effectiveStyle.backgroundTexture) {
        renderer->drawTexture(adjustedPos, getSize(), effectiveStyle.backgroundTexture);
    } else {
        renderer->drawRect(adjustedPos, getSize(), effectiveStyle.backgroundColor);
    }

    // Position label based on text alignment
    glm::vec2 labelPos = adjustedPos;
    switch (getTextAlignment()) {
        case TextAlignment::Left:
            labelPos.x += 5.0f;
            break;
        case TextAlignment::Right:
            labelPos.x += getSize().x - label_->getSize().x - 5.0f;
            break;
        case TextAlignment::Center:
            labelPos.x += (getSize().x - label_->getSize().x) / 2.0f;
            break;
        default:
            labelPos.x += (getSize().x - label_->getSize().x) / 2.0f; // Default to Center
            break;
    }
    labelPos.y += (getSize().y - label_->getSize().y) / 2.0f;
    label_->setPosition(labelPos);
    label_->render(renderer);

    dirty_ = false;
}

bool UIButton::handleInput(IMouseEvent* mouseEvent) {
    if (!mouseEvent) return false;

    bool handled = UIElement::handleInput(mouseEvent);
    if (handled) return true;

    return false; // Bubbling phase handled by base class
}

bool UIButton::handleInput(IKeyboardEvent* keyboardEvent) {
    if (!keyboardEvent) return false;

    bool handled = UIElement::handleInput(keyboardEvent);
    if (handled || !hasFocus()) return handled;

    if (keyboardEvent->getType() == EventType::KeyPress && keyboardEvent->getKeyCode() == KeyCode::Return) {
        publishEvent("click", EventType::MouseRelease);
        if (onClick_) onClick_();
        return true;
    }
    return false;
}

bool UIButton::handleInput(ITextInputEvent* textEvent) {
    return UIElement::handleInput(textEvent); // No text input for buttons
}

} // namespace ui