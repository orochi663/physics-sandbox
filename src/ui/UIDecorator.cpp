#include "UIDecorator.h"
#include <spdlog/spdlog.h>

namespace ui {

UIDecorator::UIDecorator(std::unique_ptr<UIElement> component) : component_(std::move(component)) {
    if (component_) {
        component_->setParent(this);
    }
}

void UIDecorator::setPosition(const glm::vec2& pos) {
    if (component_) component_->setPosition(pos);
}

void UIDecorator::setSize(const glm::vec2& size) {
    if (component_) component_->setSize(size);
}

void UIDecorator::render(IRenderer* renderer) {
    if (component_) component_->render(renderer);
}

bool UIDecorator::handleInput(IMouseEvent* mouseEvent) {
    if (component_) return component_->handleInput(mouseEvent);
    return false;
}

bool UIDecorator::handleInput(IKeyboardEvent* keyboardEvent) {
    if (component_) return component_->handleInput(keyboardEvent);
    return false;
}

bool UIDecorator::handleInput(ITextInputEvent* textEvent) {
    if (component_) return component_->handleInput(textEvent);
    return false;
}

UIFocusableDecorator::UIFocusableDecorator(std::unique_ptr<UIElement> component)
    : UIDecorator(std::move(component)) {}

bool UIFocusableDecorator::handleInput(IMouseEvent* mouseEvent) {
    if (!mouseEvent) return false;

    bool handled = UIDecorator::handleInput(mouseEvent);
    if (mouseEvent->getType() == EventType::MousePress && hitTest(mouseEvent->getPosition())) {
        setFocus(true);
        return true;
    }
    return handled;
}

bool UIFocusableDecorator::handleInput(IKeyboardEvent* keyboardEvent) {
    if (!keyboardEvent) return false;

    return UIDecorator::handleInput(keyboardEvent);
}

UIDraggableDecorator::UIDraggableDecorator(std::unique_ptr<UIElement> component)
    : UIDecorator(std::move(component)), isDragging_(false) {}

bool UIDraggableDecorator::handleInput(IMouseEvent* mouseEvent) {
    if (!mouseEvent) return false;

    glm::vec2 pos = mouseEvent->getPosition();
    bool isInside = hitTest(pos);

    if (mouseEvent->getType() == EventType::MousePress && isInside && mouseEvent->getButton() == MouseButton::Left) {
        isDragging_ = true;
        dragStart_ = pos - getPosition();
        return true;
    } else if (mouseEvent->getType() == EventType::MouseMove && isDragging_) {
        setPosition(pos - dragStart_);
        return true;
    } else if (mouseEvent->getType() == EventType::MouseRelease && isDragging_) {
        isDragging_ = false;
        return true;
    }

    return UIDecorator::handleInput(mouseEvent);
}

bool UIDraggableDecorator::handleInput(IKeyboardEvent* keyboardEvent) {
    return UIDecorator::handleInput(keyboardEvent);
}

} // namespace ui