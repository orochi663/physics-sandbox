#include "ui/UIElement.h"
#include "ui/UIEventBus.h"
#include <spdlog/spdlog.h>
#include <algorithm>

namespace ui {

    UIElement::UIElement() {
        registerEventHandler("styleUpdate", [this](UIElement*, EventType) { onStyleUpdate(); });
    }

    void UIElement::setPosition(const glm::vec2& pos) {
        position_ = pos;
        markDirty();
    }

    void UIElement::setSize(const glm::vec2& size) {
        size_ = size;
        markDirty();
    }

    void UIElement::setParent(UIElement* parent) {
        parent_ = parent;
    }

    void UIElement::addChild(std::unique_ptr<UIElement> child) {
        if (child) {
            child->setParent(this);
            children_.push_back(std::move(child));
            markDirty();
        }
    }

    void UIElement::removeChild(const UIElement* child) {
        children_.erase(
            std::remove_if(children_.begin(), children_.end(),
                [child](const auto& ptr) { return ptr.get() == child; }),
            children_.end()
        );
        markDirty();
    }

    void UIElement::setFocus(bool focused) {
        focused_ = focused;
        if (focused)
            spdlog::debug("Focus set to element with styleType: {}, id: {}", styleType_, id_.value_or("none"));
    }

    bool UIElement::hitTest(const glm::vec2& point) const {
        return (point.x >= position_.x && point.x <= position_.x + size_.x &&
            point.y >= position_.y && point.y <= position_.y + size_.y);
    }

    bool UIElement::handleInput(IMouseEvent* mouseEvent) {
        if (!mouseEvent) return false;
        for (auto it = children_.rbegin(); it != children_.rend(); ++it) {
            if (*it && (*it)->hitTest(mouseEvent->getPosition())) {
                if ((*it)->handleInput(mouseEvent))
                    return true;
            }
        }
        glm::vec2 pos = mouseEvent->getPosition();
        bool isInside = hitTest(pos);
        switch (mouseEvent->getType()) {
        case EventType::MouseMove:
            if (isHovered_ != isInside) {
                isHovered_ = isInside;
                markDirty();
            }
            break;
        case EventType::MousePress:
            if (isInside && mouseEvent->getButton() == MouseButton::Left) {
                isPressed_ = true;
                setFocus(true);
                markDirty();
                return true;
            }
            break;
        case EventType::MouseRelease:
            if (isPressed_ && isInside && mouseEvent->getButton() == MouseButton::Left) {
                isPressed_ = false;
                markDirty();
                publishEvent("click", mouseEvent->getType());
                return true;
            }
            isPressed_ = false;
            markDirty();
            break;
        default:
            break;
        }
        return false;
    }

    bool UIElement::handleInput(IKeyboardEvent* keyboardEvent) {
        if (!keyboardEvent) return false;
        for (auto& child : children_) {
            if (child && child->hasFocus()) {
                if (child->handleInput(keyboardEvent))
                    return true;
            }
        }
        return false;
    }

    bool UIElement::handleInput(ITextInputEvent* textEvent) {
        if (!textEvent) return false;
        for (auto& child : children_) {
            if (child && child->hasFocus()) {
                if (child->handleInput(textEvent))
                    return true;
            }
        }
        return false;
    }

    void UIElement::registerEventHandler(const std::string& eventType, std::function<void(UIElement*, EventType)> handler) {
        UIEventBus::getInstance().registerHandler(eventType, this, handler);
    }

    void UIElement::unregisterEventHandler(const std::string& eventType) {
        UIEventBus::getInstance().unregisterHandler(eventType, this);
    }

    void UIElement::publishEvent(const std::string& eventType, EventType data) {
        UIEventBus::getInstance().publish(eventType, this, data);
    }

    void UIElement::drawBackground(IRenderer* renderer) {
        const UITheme* theme = getEffectiveTheme();
        if (!theme) return;
        UIStyle style = *theme->getStyle(styleType_).get();
        if (style.backgroundTexture) {
            renderer->drawTexture(position_, size_, style.backgroundTexture.get());
        }
        else {
            renderer->drawRect(position_, size_, style.backgroundColor);
        }
    }

    const UITheme* UIElement::getEffectiveTheme() const {
        // Default behavior: if a parent exists, use parent's effective theme.
        if (parent_) {
            return (*parent_)->getEffectiveTheme();
        }
        return nullptr;
    }

    void UIElement::setTooltip(const std::string& tooltipText) {
        if (tooltipText.empty()) {
            tooltip_.reset();
        }
        else {
            if (!tooltip_)
                tooltip_ = std::make_unique<UITooltip>(tooltipText);
            else
                tooltip_->setText(tooltipText);
        }
        markDirty();
    }

    std::string UIElement::getTooltip() const {
        return tooltip_ ? tooltip_->getText() : "";
    }

} // namespace ui
