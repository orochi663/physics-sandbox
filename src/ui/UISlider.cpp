#include "ui/UISlider.h"
#include "ui/UILabel.h"
#include "ui/UICanvas.h"
#include "ui/IRenderer.h"
#include <spdlog/spdlog.h>
#include <algorithm>
#include <glm/glm.hpp>
#include <format>

namespace ui {

    std::unique_ptr<UISlider> UISlider::create(float minValue, float maxValue, float initialValue) {
        return std::unique_ptr<UISlider>(new UISlider(minValue, maxValue, initialValue));
    }

    UISlider::UISlider(float minValue, float maxValue, float initialValue)
        : minValue_(minValue), maxValue_(maxValue), value_(initialValue)
    {
        styleType_ = "slider";
        size_ = glm::vec2(100.0f, 20.0f);
        valueLabel_ = UILabel::create();
        valueLabel_->setStyleType("sliderLabel");
		valueLabel_->setParent(this);
        setTextAlignment(TextAlignment::CenterTop);
        updateHandlePosition();
        registerEventHandler("styleUpdate", [this](UIElement*, EventType) { onStyleUpdate(); });
    }

    void UISlider::updateHandlePosition() {
        float t = (value_ - minValue_) / (maxValue_ - minValue_);
        handlePos_.x = position_.x + t * (size_.x - handleSize_.x);
        handlePos_.y = position_.y + (size_.y - handleSize_.y) / 2.0f;
        valueLabel_->setText(std::format("{:.1f}", value_));
        markDirty();
    }

    void UISlider::setValue(float value) {
        value_ = std::clamp(value, minValue_, maxValue_);
        updateHandlePosition();
        if (onValueChanged_) onValueChanged_(value_);
    }

    void UISlider::render(IRenderer* renderer) {
        if (!renderer || !isDirty()) return;
        drawBackground(renderer);
        const UITheme* theme = nullptr;
        if (auto* canvas = dynamic_cast<UICanvas*>(parent_.value_or(nullptr))) {
            theme = canvas->getEffectiveTheme();
        }
        if (!theme) return;
        UIStyle style = theme->getStyle(styleType_);
        std::unordered_map<std::string, bool> states = { {"hovered", isHovered_}, {"pressed", isDragging_} };
        UIStyle effectiveStyle = style.computeEffectiveStyle(states);
        renderer->drawRect(position_, size_, effectiveStyle.backgroundColor);
        UIStyle handleStyle = theme->getStyle("sliderHandle");
        renderer->drawRect(handlePos_, handleSize_, handleStyle.backgroundColor);
        glm::vec2 labelPos = position_ + glm::vec2((size_.x - valueLabel_->getSize().x) / 2.0f, -valueLabel_->getSize().y - 5.0f);
        valueLabel_->setPosition(labelPos);
        valueLabel_->render(renderer);
        markDirty();
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
            }
            else if (isInsideTrack) {
                float t = (pos.x - position_.x) / size_.x;
                setValue(minValue_ + t * (maxValue_ - minValue_));
                isDragging_ = true;
                dragStart_ = pos - handlePos_;
                return true;
            }
        }
        else if (mouseEvent->getType() == EventType::MouseMove && isDragging_) {
            float newX = pos.x - dragStart_.x;
            newX = std::clamp(newX, position_.x, position_.x + size_.x - handleSize_.x);
            float t = (newX - position_.x) / (size_.x - handleSize_.x);
            setValue(minValue_ + t * (maxValue_ - minValue_));
            return true;
        }
        else if (mouseEvent->getType() == EventType::MouseRelease) {
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
            }
            else if (keyboardEvent->getKeyCode() == KeyCode::Right || keyboardEvent->getKeyCode() == KeyCode::Up) {
                setValue(value_ + step);
                return true;
            }
        }
        return UIElement::handleInput(keyboardEvent);
    }

    void UISlider::onStyleUpdate() {
        markDirty();
    }

} // namespace ui
