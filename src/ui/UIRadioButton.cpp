#include "ui/UIRadioButton.h"
#include "ui/UILabel.h"
#include "ui/UICanvas.h"
#include "ui/IRenderer.h"
#include <spdlog/spdlog.h>
#include <glm/glm.hpp>

namespace ui {

    std::unique_ptr<UIRadioButton> UIRadioButton::create(const std::string& labelText) {
        return std::unique_ptr<UIRadioButton>(new UIRadioButton(labelText));
    }

    UIRadioButton::UIRadioButton(const std::string& labelText) {
        styleType_ = "radioButton";
        size_ = glm::vec2(20.0f, 20.0f);
        label_ = UILabel:;create (labelText);
		label_->setParent(this);
        label_->setStyleType("radioButtonLabel");
        setTextAlignment(TextAlignment::Right);
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
        if (!renderer || !isDirty()) return;

        drawBackground(renderer);

        const UITheme* theme = nullptr;
        if (auto* canvas = dynamic_cast<UICanvas*>(parent_.value_or(nullptr))) {
            theme = canvas->getEffectiveTheme();
        }
        if (!theme) return;
        UIStyle style = theme->getStyle(styleType_);
        std::unordered_map<std::string, bool> states = { {"hovered", isHovered_}, {"pressed", isPressed_}, {"checked", checked_} };
        UIStyle effectiveStyle = style.computeEffectiveStyle(states);

        renderer->drawRect(position_, size_, effectiveStyle.backgroundColor);
        if (checked_) {
            glm::vec2 center = position_ + size_ * 0.5f;
            float radius = size_.x * 0.25f;
            renderer->drawLine(center - glm::vec2(radius, radius), center + glm::vec2(radius, radius), effectiveStyle.textColor);
        }
        glm::vec2 labelPos = position_ + glm::vec2(size_.x + 5.0f, 0.0f);
        label_->setPosition(labelPos);
        label_->render(renderer);

        markDirty();
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

    void UIRadioButton::onStyleUpdate() {
        if (label_) {
            label_->onStyleUpdate();
        }
        markDirty();
    }

} // namespace ui
