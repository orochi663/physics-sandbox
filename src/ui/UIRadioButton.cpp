#include "UIRadioButton.h"
#include "UILabel.h"
#include "UICanvas.h"
#include <spdlog/spdlog.h>

namespace ui {

std::unique_ptr<UIRadioButton> UIRadioButton::create(const std::string& labelText) {
    return std::unique_ptr<UIRadioButton>(new UIRadioButton(labelText));
}

UIRadioButton::UIRadioButton(const std::string& labelText) {
    styleType_ = "radioButton";
    size_ = glm::vec2(20.0f, 20.0f);
    label_ = std::make_unique<UILabel>(this);
    label_->setText(labelText);
    label_->setStyleType("radioButtonLabel");
    setTextAlignment(TextAlignment::Right); // Default to Right as per original design
    registerEventHandler("styleUpdate", [this](UIElement*, EventType) { onStyleUpdate(); });
}

void UIRadioButton::setChecked(bool checked) {
    if (checked_ != checked) {
        checked_ = checked;
        markDirty();
        publishEvent("radioButtonChecked", EventType::MousePress);
        if (onChecked_) onChecked_(checked_);
    }
}

void UIRadioButton::render(IRenderer* renderer) {
    if (!renderer || !dirty_) return;

    const UITheme* theme = nullptr;
    if (auto* canvas = dynamic_cast<UICanvas*>(parent_.value_or(nullptr))) {
        theme = canvas->getEffectiveTheme();
    }
    if (!theme) return;

    UIStyle style = theme->getStyle(styleType_);
    std::unordered_map<std::string, bool> states = {{"hovered", isHovered_}, {"pressed", isPressed_}, {"checked", checked_}};
    UIStyle effectiveStyle = style.computeEffectiveStyle(states);

    // Render radio button circle
    nvgBeginPath(renderer->getNVGContext());
    nvgCircle(renderer->getNVGContext(), position_.x + size_.x / 2.0f, position_.y + size_.y / 2.0f, size_.x / 2.0f);
    nvgFillColor(renderer->getNVGContext(),
                 nvgRGBAf(effectiveStyle.backgroundColor.r, effectiveStyle.backgroundColor.g,
                          effectiveStyle.backgroundColor.b, effectiveStyle.backgroundColor.a));
    nvgFill(renderer->getNVGContext());

    if (checked_) {
        nvgBeginPath(renderer->getNVGContext());
        nvgCircle(renderer->getNVGContext(), position_.x + size_.x / 2.0f, position_.y + size_.y / 2.0f, size_.x / 4.0f);
        nvgFillColor(renderer->getNVGContext(), nvgRGBA(255, 255, 255, 255));
        nvgFill(renderer->getNVGContext());
    }

    // Position and render label based on text alignment
    glm::vec2 labelPos;
    switch (getTextAlignment()) {
        case TextAlignment::Left:
            labelPos = glm::vec2(position_.x - label_->getSize().x - 5.0f, position_.y);
            break;
        case TextAlignment::Right:
            labelPos = glm::vec2(position_.x + size_.x + 5.0f, position_.y);
            break;
        default:
            labelPos = glm::vec2(position_.x + size_.x + 5.0f, position_.y); // Default to Right
            break;
    }
    label_->setPosition(labelPos);
    label_->render(renderer);

    dirty_ = false;
}

bool UIRadioButton::handleInput(IMouseEvent* mouseEvent) {
    if (!mouseEvent) return false;

    bool handled = UIElement::handleInput(mouseEvent);
    if (handled && mouseEvent->getType() == EventType::MouseRelease && hitTest(mouseEvent->getPosition())) {
        setChecked(true);
        return true;
    }
    return handled;
}

bool UIRadioButton::handleInput(IKeyboardEvent* keyboardEvent) {
    if (!keyboardEvent || !hasFocus()) return false;

    if (keyboardEvent->getType() == EventType::KeyPress && keyboardEvent->getKeyCode() == KeyCode::Space) {
        setChecked(true);
        return true;
    }
    return UIElement::handleInput(keyboardEvent);
}

} // namespace ui