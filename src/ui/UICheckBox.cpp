#include "UICheckBox.h"
#include "UILabel.h"
#include "UICanvas.h"
#include <spdlog/spdlog.h>

namespace ui {

std::unique_ptr<UICheckBox> UICheckBox::create(const std::string& labelText) {
    return std::unique_ptr<UICheckBox>(new UICheckBox(labelText));
}

UICheckBox::UICheckBox(const std::string& labelText) {
    styleType_ = "checkBox";
    size_ = glm::vec2(20.0f, 20.0f);
    label_ = std::make_unique<UILabel>(this);
    label_->setText(labelText);
    label_->setStyleType("checkBoxLabel");
    setTextAlignment(TextAlignment::Right); // Default to Right as per original design
    registerEventHandler("styleUpdate", [this](UIElement*, EventType) { onStyleUpdate(); });
}

void UICheckBox::setChecked(bool checked) {
    checked_ = checked;
    markDirty();
    if (onChecked_) onChecked_(checked_);
}

void UICheckBox::render(IRenderer* renderer) {
    if (!renderer || !dirty_) return;

    const UITheme* theme = nullptr;
    if (auto* canvas = dynamic_cast<UICanvas*>(parent_.value_or(nullptr))) {
        theme = canvas->getEffectiveTheme();
    }
    if (!theme) return;

    UIStyle style = theme->getStyle(styleType_);
    std::unordered_map<std::string, bool> states = {{"hovered", isHovered_}, {"pressed", isPressed_}, {"checked", checked_}};
    UIStyle effectiveStyle = style.computeEffectiveStyle(states);

    // Render checkbox
    renderer->drawRect(position_, size_, effectiveStyle.backgroundColor);
    if (checked_) {
        renderer->drawLine(position_ + glm::vec2(2.0f, 2.0f), position_ + size_ - glm::vec2(2.0f, 2.0f), effectiveStyle.textColor);
        renderer->drawLine(position_ + glm::vec2(size_.x - 2.0f, 2.0f), position_ + glm::vec2(2.0f, size_.y - 2.0f), effectiveStyle.textColor);
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

bool UICheckBox::handleInput(IMouseEvent* mouseEvent) {
    if (!mouseEvent) return false;

    bool handled = UIElement::handleInput(mouseEvent);
    if (handled && mouseEvent->getType() == EventType::MouseRelease && hitTest(mouseEvent->getPosition())) {
        setChecked(!checked_);
        return true;
    }
    return handled;
}

bool UICheckBox::handleInput(IKeyboardEvent* keyboardEvent) {
    if (!keyboardEvent || !hasFocus()) return false;

    if (keyboardEvent->getType() == EventType::KeyPress && keyboardEvent->getKeyCode() == KeyCode::Space) {
        setChecked(!checked_);
        return true;
    }
    return UIElement::handleInput(keyboardEvent);
}

} // namespace ui