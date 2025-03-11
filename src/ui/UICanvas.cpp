#include "UICanvas.h"
#include "UIScrollbar.h"
#include "UIXYLayout.h"
#include <spdlog/spdlog.h>
#include "CanvasFactory.h"


namespace ui {

UICanvas::UICanvas(const std::string& styleType, int zIndex) : zIndex_(zIndex) {
    styleType_ = styleType;
    layout_ = std::make_unique<UIXYLayout>();
    registerEventHandler("styleUpdate", [this](UIElement*, EventType) { onStyleUpdate(); });
}

void UICanvas::addChild(std::unique_ptr<UIElement> child) {
    if (child) {
        child->setParent(this);
        children_.push_back(std::move(child));
        updateLayout();
        markDirty();
    }
}

void UICanvas::render(IRenderer* renderer) {
    if (!renderer || !isVisible_ || !dirty_) return;

    const UITheme* theme = getEffectiveTheme();
    if (!theme) return;

    UIStyle style = theme->getStyle(styleType_);
    std::unordered_map<std::string, bool> states = {{"hovered", isHovered_}, {"pressed", isPressed_}};
    UIStyle effectiveStyle = style.computeEffectiveStyle(states);

    renderer->setClipRect(getPosition(), getSize());

    glm::vec2 cumulativeOffset = getCumulativeScrollOffset();
    glm::vec2 adjustedPos = getPosition() - cumulativeOffset;

    if (effectiveStyle.backgroundTexture) {
        renderer->drawTexture(adjustedPos, getSize(), effectiveStyle.backgroundTexture);
    } else {
        renderer->drawRect(adjustedPos, getSize(), effectiveStyle.backgroundColor);
    }

    for (const auto& child : children_) {
        if (child) {
            glm::vec2 childPos = child->getPosition() - cumulativeOffset;
            child->setPosition(childPos);
            child->render(renderer);
            child->setPosition(childPos + cumulativeOffset);
        }
    }

    for (const auto& scrollbar : scrollbars_) {
        if (scrollbar) scrollbar->render(renderer);
    }

    renderer->resetClipRect();
    dirty_ = false;
}

bool UICanvas::handleInput(IMouseEvent* mouseEvent) {
    if (!mouseEvent || !isVisible_) return false;

    bool handled = false;
    glm::vec2 pos = mouseEvent->getPosition();

    // Capturing phase
    for (auto& scrollbar : scrollbars_) {
        if (scrollbar && scrollbar->hitTest(pos)) {
            handled = scrollbar->handleInput(mouseEvent);
            if (handled) return true;
        }
    }

    for (auto& child : children_) {
        if (child && child->hitTest(pos)) {
            handled = child->handleInput(mouseEvent);
            if (handled) return true;
        }
    }

    // Bubbling phase
    bool isInside = hitTest(pos);

    switch (mouseEvent->getType()) {
        case EventType::MouseMove:
            isHovered_ = isInside;
            markDirty();
            break;
        case EventType::MousePress:
            if (isInside && mouseEvent->getButton() == MouseButton::Left) {
                isPressed_ = true;
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
            break;
        case EventType::MouseWheel:
            if (isInside) {
                glm::vec2 delta = mouseEvent->getWheelDelta();
                scrollOffset_.y -= delta.y * 10.0f;
                updateLayout();
                markDirty();
                return true;
            }
            break;
    }
    return false;
}

bool UICanvas::handleInput(IKeyboardEvent* keyboardEvent) {
    if (!keyboardEvent || !isVisible_) return false;

    bool handled = false;
    for (auto& child : children_) {
        if (child && child->hasFocus()) {
            handled = child->handleInput(keyboardEvent);
            if (handled) return true;
        }
    }

    for (auto& scrollbar : scrollbars_) {
        if (scrollbar && scrollbar->hasFocus()) {
            handled = scrollbar->handleInput(keyboardEvent);
            if (handled) return true;
        }
    }
    return false;
}

bool UICanvas::handleInput(ITextInputEvent* textEvent) {
    if (!textEvent || !isVisible_) return false;

    bool handled = false;
    for (auto& child : children_) {
        if (child && child->hasFocus()) {
            handled = child->handleInput(textEvent);
            if (handled) return true;
        }
    }
    return false;
}

void UICanvas::onStyleUpdate() {
    for (auto& child : children_) if (child) child->onStyleUpdate();
    for (auto& scrollbar : scrollbars_) if (scrollbar) scrollbar->onStyleUpdate();
    updateLayout();
    markDirty();
}

void UICanvas::addChild(std::unique_ptr<UIElement> child) {
    if (child) {
        child->setParent(this);
        children_.push_back(std::move(child));
        updateLayout();
        markDirty();
    }
}

void UICanvas::removeChild(const UIElement* child) {
    children_.erase(
        std::remove_if(children_.begin(), children_.end(),
                       [child](const auto& ptr) { return ptr.get() == child; }),
        children_.end()
    );
    updateLayout();
    markDirty();
}

const std::vector<std::unique_ptr<UIElement>>& UICanvas::getChildren() const {
    return children_;
}

void UICanvas::addScrollbar(std::unique_ptr<UIScrollbar> scrollbar) {
    if (scrollbar) {
        scrollbar->attachToCanvas(this);
        scrollbars_.push_back(std::move(scrollbar));
        markDirty();
    }
}

void UICanvas::setScrollOffset(float xOffset, float yOffset) {
    scrollOffset_.x = xOffset;
    scrollOffset_.y = yOffset;
    updateLayout();
    markDirty();
}

void UICanvas::setLayout(std::unique_ptr<UILayout> layout) {
    layout_ = std::move(layout);
    updateLayout();
    markDirty();
}

void UICanvas::updateLayout() {
    if (layout_) {
        contentSize_ = layout_->arrange(this, children_);
        float maxXOffset = std::max(0.0f, contentSize_.x - getSize().x);
        float maxYOffset = std::max(0.0f, contentSize_.y - getSize().y);
        scrollOffset_.x = std::clamp(scrollOffset_.x, 0.0f, maxXOffset);
        scrollOffset_.y = std::clamp(scrollOffset_.y, 0.0f, maxYOffset);
    } else {
        contentSize_ = getSize();
    }
    markDirty();
}

glm::vec2 UICanvas::getCumulativeScrollOffset() const {
    glm::vec2 offset = scrollOffset_;
    const UIElement* current = this;
    while (current->getParent()) {
        if (const auto* parentCanvas = dynamic_cast<const UICanvas*>(current->getParent().value())) {
            offset += parentCanvas->scrollOffset_;
        }
        current = current->getParent().value();
    }
    return offset;
}

} // namespace ui