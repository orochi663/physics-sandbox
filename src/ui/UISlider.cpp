#include "UISlider.h"
#include "UILabel.h"
#include "UICanvas.h"
#include <spdlog/spdlog.h>

namespace ui {

std::unique_ptr<UISlider> UISlider::create(float minValue, float maxValue, float initialValue) {
    return std::unique_ptr<UISlider>(new UISlider(minValue, maxValue, initialValue));
}

UISlider::UISlider(float minValue, float maxValue, float initialValue)
    : minValue_(minValue), maxValue_(maxValue), value_(initialValue) {
    styleType_ = "slider";
    size_ = glm::vec2(100.0f, 20.0f);
    valueLabel_ = std::make_unique<UILabel>(this);
    valueLabel_->setStyleType("sliderLabel");
    setTextAlignment(TextAlignment::CenterTop); // Default to CenterTop as per original design
    updateHandlePosition();
    registerEventHandler("styleUpdate", [this](UIElement*, EventType) { onStyleUpdate(); });
}

void UISlider::updateHandlePosition() {
    float t = (value_ - minValue_) / (maxValue_ - minValue_);
    handlePos_.x = position_.x + t * (size_.x - handleSize_.x);
    handlePos_.y = position_.y + (size_.y - handleSize_.y) / 2.0f;
    valueLabel_->setText(std::to_string(static_cast<int>(value_)));
    markDirty();
}

void UISlider::setValue(float value) {
    value_ = std::clamp(value, minValue_, maxValue_);
    updateHandlePosition();
    if (onValueChanged_) onValueChanged_(value_);
}

void UISlider::render(IRenderer* renderer) {
    if (!renderer || !dirty_) return;

    const UITheme* theme = nullptr;
    if (auto* canvas = dynamic_cast<UICanvas*>(parent_.value_or(nullptr))) {
        theme = canvas->getEffectiveTheme();
    }
    if (!theme) return;

    // Render track
    UIStyle trackStyle = theme->getStyle(styleType_);
    std::unordered_map<std::string, bool> states = {{"hovered", isHovered_}, {"pressed", isDragging_}};
    UIStyle effectiveTrackStyle = trackStyle.computeEffectiveStyle(states);
    renderer->drawRect(position_, size_, effectiveTrackStyle.backgroundColor);

    // Render handle
    UIStyle handleStyle = theme->getStyle("sliderHandle");
    renderer->drawRect(handlePos_, handleSize_, handleStyle.backgroundColor);

    // Position and render value label based on text alignment
    glm::vec2 labelPos;
    switch (getTextAlignment()) {
        case TextAlignment::Left:
            labelPos = glm::vec2(position_.x - valueLabel_->getSize().x - 5.0f, position_.y);
            break;
        case TextAlignment::Right:
            labelPos = glm::vec2(position_.x + size_.x + 5.0f, position_.y);
            break;
        case TextAlignment::CenterTop:
            labelPos = glm::vec2(position_.x + size_.x / 2.0f - valueLabel_->getSize().x / 2.0f, position_.y - valueLabel_->getSize().y - 5.0f);
            break;
        case TextAlignment::CenterBottom:
            labelPos = glm::vec2(position_.x + size_.x / 2.0f - valueLabel_->getSize().x / 2.0f, position_.y + size_.y + 5.0f);
            break;
        default:
            labelPos = glm::vec2(position_.x + size_.x / 2.0f - valueLabel_->getSize().x / 2.0f, position_.y - valueLabel_->getSize().y - 5.0f); // Default to CenterTop
            break;
    }
    valueLabel_->setPosition(labelPos);
    valueLabel_->render(renderer);

    dirty_ = false;
}

bool UISlider::handleInput(IMouseEvent* mouseEvent) {
    if (!mouseEvent) return false;

    glm::vec2 pos = mouseEvent->getPosition();
    bool isInsideHandle = (pos.x >= handlePos_.x && pos.x <= handlePos_.x + handleSize_.x &&
                           pos.y >= handlePos_.y && pos.y <= handlePos_.y + handleSize_.y);
    bool isInsideTrack = hitTest(pos);

    if (mouseEvent->getType() == EventType::MousePress && mouseEvent->getButton() == MouseButton::Left) {
        if (isInsideHandle) {
            isDragging_ = true;
            dragStart_ = pos - handlePos_;
            return true;
        } else if (isInsideTrack) {
            float t = (pos.x - position_.x) / size_.x;
            setValue(minValue_ + t * (maxValue_ - minValue_));
            isDragging_ = true;
            dragStart_ = pos - handlePos_;
            return true;
        }
    } else if (mouseEvent->getType() == EventType::MouseMove && isDragging_) {
        float newX = pos.x - dragStart_.x;
        newX = std::clamp(newX, position_.x, position_.x + size_.x - handleSize_.x);
        float t = (newX - position_.x) / (size_.x - handleSize_.x);
        setValue(minValue_ + t * (maxValue_ - minValue_));
        return true;
    } else if (mouseEvent->getType() == EventType::MouseRelease) {
        isDragging_ = false;
        markDirty();
        return true;
    }

    return UIElement::handleInput(mouseEvent);
}

bool UISlider::handleInput(IKeyboardEvent* keyboardEvent) {
    if (!keyboardEvent || !hasFocus()) return false;

    if (keyboardEvent->getType() == EventType::KeyPress) {
        float step = (maxValue_ - minValue_) / 100.0f;
        if (keyboardEvent->getKeyCode() == KeyCode::Left || keyboardEvent->getKeyCode() == KeyCode::Down) {
            setValue(value_ - step);
            return true;
        } else if (keyboardEvent->getKeyCode() == KeyCode::Right || keyboardEvent->getKeyCode() == KeyCode::Up) {
            setValue(value_ + step);
            return true;
        }
    }
    return UIElement::handleInput(keyboardEvent);
}

} // namespace ui