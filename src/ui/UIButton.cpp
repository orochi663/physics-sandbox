#include "ui/UIButton.h"
#include "ui/UILabel.h"
#include "ui/UICanvas.h"
#include "ui/IRenderer.h"
#include "ui/ITexture.h"
#include <spdlog/spdlog.h>
#include <glm/glm.hpp>
#include <algorithm>

namespace ui {

    std::unique_ptr<UIButton> UIButton::create(const std::string& labelText) {
        return std::unique_ptr<UIButton>(new UIButton(labelText));
    }

    UIButton::UIButton(const std::string& labelText) {
        styleType_ = "button";
        // Use factory method to create a UILabel.
        label_ = UILabel::create(labelText);
        label_->setParent(this);
        label_->setStyleType("buttonLabel");
        setTextAlignment(TextAlignment::Center);
        registerEventHandler("styleUpdate", [this](UIElement*, EventType) { onStyleUpdate(); });
    }

    void UIButton::setText(const std::string& text) {
        if (label_) {
            label_->setText(text);
            markDirty();
        }
    }

    std::string UIButton::getText() const {
        return label_ ? label_->getText() : "";
    }

    void UIButton::setIcon(const std::shared_ptr<ITexture>& icon) {
        icon_ = icon;
        markDirty();
    }

    void UIButton::render(IRenderer* renderer) {
        if (!renderer || !isDirty()) return;

        // Draw the background.
        drawBackground(renderer);

        const UITheme* theme = nullptr;
        if (auto* canvas = dynamic_cast<UICanvas*>(parent_.value_or(nullptr))) {
            theme = canvas->getEffectiveTheme();
        }
        if (!theme) return;

        UIStyle style = *theme->getStyle(styleType_).get();
        std::unordered_map<std::string, bool> states = { {"hovered", isHovered_}, {"pressed", isPressed_} };
        UIStyle effectiveStyle = style.computeEffectiveStyle(states);

        if (icon_ && getText().empty()) {
            glm::vec2 iconSize = { static_cast<float>(icon_->getWidth()), static_cast<float>(icon_->getHeight()) };
            glm::vec2 iconPos = position_ + (size_ - iconSize) * 0.5f;
            renderer->drawTexture(iconPos, iconSize, icon_.get());
        }
        else if (icon_ && !getText().empty()) {
            glm::vec2 iconSize = { static_cast<float>(icon_->getWidth()), static_cast<float>(icon_->getHeight()) };
            float padding = 5.0f;
            glm::vec2 iconPos = position_ + glm::vec2(padding, (size_.y - iconSize.y) * 0.5f);
            renderer->drawTexture(iconPos, iconSize, icon_.get());
            glm::vec2 labelPos = position_ + glm::vec2(iconSize.x + 2 * padding, (size_.y - label_->getSize().y) * 0.5f);
            label_->setPosition(labelPos);
            label_->render(renderer);
        }
        else {
            glm::vec2 textSize = renderer->measureText(getText(), style.fontSize);
            glm::vec2 labelPos = position_ + (size_ - textSize) * 0.5f;
            if (label_) {
                label_->setPosition(labelPos);
                label_->render(renderer);
            }
        }
        markDirty();
    }

    bool UIButton::handleInput(IMouseEvent* mouseEvent) {
        if (!mouseEvent) return false;
        bool handled = UIElement::handleInput(mouseEvent);
        return handled;
    }

    bool UIButton::handleInput(IKeyboardEvent* keyboardEvent) {
        if (!keyboardEvent) return false;
        if (keyboardEvent->getType() == EventType::KeyPress && keyboardEvent->getKeyCode() == KeyCode::Return) {
            publishEvent("click", EventType::MouseRelease);
            if (onClick_) onClick_();
            return true;
        }
        return UIElement::handleInput(keyboardEvent);
    }

    void UIButton::onStyleUpdate() {
        if (label_) {
            label_->onStyleUpdate();
        }
        markDirty();
    }

} // namespace ui
