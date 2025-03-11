#include "UIManager.h"
#include "UICanvas.h"
#include "UIDockable.h"
#include "UIElement.h"
#include <algorithm>
#include <spdlog/spdlog.h>

namespace ui {

UIManager& UIManager::getInstance() {
    static UIManager instance;
    return instance;
}

UIManager::UIManager() {}

UIManager::~UIManager() {
    canvases_.clear();
    dockables_.clear();
}

void UIManager::processInput(void* rawEvent) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!translator_) {
        spdlog::warn("No input translator set in UIManager");
        return;
    }

    std::unique_ptr<IInputEvent> event = translator_->translate(rawEvent);
    if (!event) return;

    switch (event->getType()) {
        case EventType::MouseMove:
        case EventType::MousePress:
        case EventType::MouseRelease:
            dispatchMouseEvent(static_cast<IMouseEvent*>(event.get()));
            break;
        case EventType::KeyPress:
        case EventType::KeyRelease:
            dispatchKeyboardEvent(static_cast<IKeyboardEvent*>(event.get()));
            break;
        case EventType::TextInput:
            dispatchTextInputEvent(static_cast<ITextInputEvent*>(event.get()));
            break;
    }
}

void UIManager::setGlobalTheme(std::unique_ptr<UITheme> theme) {
    std::lock_guard<std::mutex> lock(mutex_);
    globalTheme_ = std::move(theme);
    applyTheme();
}

void UIManager::applyTheme() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (globalTheme_) {
        for (auto& canvas : canvases_) {
            if (canvas) {
                canvas->applyTheme(globalTheme_.get());
            }
        }
    }
}

void UIManager::setFocusedElement(UIElement* element) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (focusedElement_ != element) {
        if (focusedElement_) focusedElement_->setFocus(false);
        focusedElement_ = element;
        if (focusedElement_) focusedElement_->setFocus(true);
    }
}

void UIManager::focusNext() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!focusedElement_) {
        if (!canvases_.empty() && canvases_[0]) {
            setFocusedElement(findNextFocusable(canvases_[0].get(), false));
        }
        return;
    }
    UIElement* next = findNextFocusable(focusedElement_);
    if (next) setFocusedElement(next);
}

void UIManager::focusPrevious() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!focusedElement_) {
        if (!canvases_.empty() && canvases_[0]) {
            setFocusedElement(findPreviousFocusable(canvases_[0].get(), false));
        }
        return;
    }
    UIElement* prev = findPreviousFocusable(focusedElement_);
    if (prev) setFocusedElement(prev);
}

void UIManager::moveFocusToNextCanvas() {
    std::lock_guard<std::mutex> lock(mutex_);
    if (canvases_.empty()) return;

    auto it = std::find_if(canvases_.begin(), canvases_.end(),
                           [this](const auto& canvas) { return canvas.get() == getCanvasForElement(focusedElement_); });
    if (it != canvases_.end()) {
        auto nextIt = it + 1;
        if (nextIt == canvases_.end()) nextIt = canvases_.begin();
        if (*nextIt) {
            setFocusedElement(findNextFocusable((*nextIt).get(), false));
        }
    }
}

void UIManager::addCanvas(std::unique_ptr<UICanvas> canvas) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (canvas) {
        canvases_.push_back(std::move(canvas));
        if (auto* dockable = dynamic_cast<UIDockable*>(canvases_.back().get())) {
            dockables_.push_back(dockable);
        }
        applyTheme();
    }
}

void UIManager::removeCanvas(const UICanvas* canvas) {
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = std::remove_if(canvases_.begin(), canvases_.end(),
                             [canvas](const auto& ptr) { return ptr.get() == canvas; });
    canvases_.erase(it, canvases_.end());

    auto dockIt = std::remove_if(dockables_.begin(), dockables_.end(),
                                 [canvas](UIDockable* dockable) { return dynamic_cast<UICanvas*>(dockable) == canvas; });
    dockables_.erase(dockIt, dockables_.end());
}

void UIManager::update() {
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto& canvas : canvases_) {
        if (canvas) canvas->update();
    }
}

void UIManager::render(IRenderer* renderer) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!renderer) return;

    // Sort canvases by zIndex for proper rendering order
    std::vector<UICanvas*> sortedCanvases;
    for (auto& canvas : canvases_) {
        if (canvas) sortedCanvases.push_back(canvas.get());
    }
    std::sort(sortedCanvases.begin(), sortedCanvases.end(),
              [](const UICanvas* a, const UICanvas* b) { return a->getZIndex() < b->getZIndex(); });

    for (auto* canvas : sortedCanvases) {
        if (canvas && canvas->isVisible()) {
            canvas->render(renderer);
        }
    }
}

void UIManager::setInputTranslator(std::unique_ptr<IInputTranslator> translator) {
    std::lock_guard<std::mutex> lock(mutex_);
    translator_ = std::move(translator);
}

void UIManager::handleDockableDragging(UIDockable* dockable, const glm::vec2& position) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!dockable || !dockable->isDragging()) return;

    updateDockableSnapping(dockable, position);
}

void UIManager::handleDockableRelease(UIDockable* dockable) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!dockable) return;

    DockPosition newPosition = checkDockableSnapping(dockable, dockable->getPosition());
    if (newPosition != DockPosition::None) {
        dockable->setDockPosition(newPosition);
    }
    dockable->setDragging(false);
}

bool UIManager::dispatchMouseEvent(IMouseEvent* mouseEvent) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!mouseEvent) return false;

    // Reverse order for top-down (capturing phase)
    for (auto it = canvases_.rbegin(); it != canvases_.rend(); ++it) {
        if (*it && (*it)->handleInput(mouseEvent)) {
            return true;
        }
    }
    return false;
}

bool UIManager::dispatchKeyboardEvent(IKeyboardEvent* keyboardEvent) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!keyboardEvent) return false;

    if (focusedElement_) {
        return focusedElement_->handleInput(keyboardEvent);
    }
    return false;
}

bool UIManager::dispatchTextInputEvent(ITextInputEvent* textEvent) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!textEvent) return false;

    if (focusedElement_) {
        return focusedElement_->handleInput(textEvent);
    }
    return false;
}

UIElement* UIManager::findNextFocusable(UIElement* current, bool withinScope) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!current) return nullptr;

    UICanvas* canvas = getCanvasForElement(current);
    if (!canvas) return nullptr;

    const auto& children = canvas->getChildren();
    auto it = std::find_if(children.begin(), children.end(),
                           [current](const auto& child) { return child.get() == current; });
    if (it == children.end()) return nullptr;

    auto nextIt = it + 1;
    while (nextIt != children.end()) {
        if ((*nextIt)->getFocusPriority() > 0) return nextIt->get();
        ++nextIt;
    }

    if (!withinScope && canvas != canvases_.front().get()) {
        auto canvasIt = std::find_if(canvases_.begin(), canvases_.end(),
                                     [canvas](const auto& c) { return c.get() == canvas; });
        if (canvasIt != canvases_.begin()) {
            --canvasIt;
            if (*canvasIt) {
                const auto& prevChildren = (*canvasIt)->getChildren();
                for (auto rit = prevChildren.rbegin(); rit != prevChildren.rend(); ++rit) {
                    if ((*rit)->getFocusPriority() > 0) return rit->get();
                }
            }
        }
    }

    return nullptr;
}

UIElement* UIManager::findPreviousFocusable(UIElement* current, bool withinScope) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!current) return nullptr;

    UICanvas* canvas = getCanvasForElement(current);
    if (!canvas) return nullptr;

    const auto& children = canvas->getChildren();
    auto it = std::find_if(children.begin(), children.end(),
                           [current](const auto& child) { return child.get() == current; });
    if (it == children.end()) return nullptr;

    auto prevIt = it - 1;
    while (prevIt >= children.begin()) {
        if ((*prevIt)->getFocusPriority() > 0) return prevIt->get();
        --prevIt;
    }

    if (!withinScope && canvas != canvases_.back().get()) {
        auto canvasIt = std::find_if(canvases_.begin(), canvases_.end(),
                                     [canvas](const auto& c) { return c.get() == canvas; });
        if (canvasIt != canvases_.end() - 1) {
            ++canvasIt;
            if (*canvasIt) {
                const auto& nextChildren = (*canvasIt)->getChildren();
                for (auto rit = nextChildren.rbegin(); rit != nextChildren.rend(); ++rit) {
                    if ((*rit)->getFocusPriority() > 0) return rit->get();
                }
            }
        }
    }

    return nullptr;
}

UICanvas* UIManager::getCanvasForElement(const UIElement* element) const {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!element) return nullptr;

    for (const auto& canvas : canvases_) {
        if (canvas) {
            const auto* parent = element->getParent();
            while (parent) {
                if (parent == canvas.get()) return canvas.get();
                parent = parent->getParent();
            }
        }
    }
    return nullptr;
}

void UIManager::updateDockableSnapping(UIDockable* dockable, const glm::vec2& position) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!dockable) return;

    // Update position temporarily for snapping check
    glm::vec2 originalPos = dockable->getPosition();
    dockable->setPosition(position);

    IRenderer* renderer = nullptr; // Assume access to renderer (e.g., via global or UICanvas)
    glm::vec2 screenSize = (renderer) ? renderer->getScreenSize() : glm::vec2(1280.0f, 720.0f);

    DockPosition newPosition = checkDockableSnapping(dockable, position, screenSize);
    if (newPosition != DockPosition::None) {
        dockable->setDockPosition(newPosition);
    } else {
        dockable->setDockPosition(DockPosition::None); // Ensure floating state
    }

    // Restore original position if not docked
    if (dockable->getDockPosition() == DockPosition::None) {
        dockable->setPosition(originalPos);
    }
}

DockPosition UIManager::checkDockableSnapping(UIDockable* dockable, const glm::vec2& position, const glm::vec2& screenSize) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!dockable) return DockPosition::None;

    const float snapThreshold = 20.0f;

    // Check screen edges
    if (position.y < snapThreshold) return DockPosition::Top;
    if (position.y + dockable->getSize().y > screenSize.y - snapThreshold) return DockPosition::Bottom;
    if (position.x < snapThreshold) return DockPosition::Left;
    if (position.x + dockable->getSize().x > screenSize.x - snapThreshold) return DockPosition::Right;

    // Check other dockables
    for (UIDockable* other : dockables_) {
        if (other == dockable || other->getDockPosition() == DockPosition::None) continue;

        glm::vec2 otherPos = other->getPosition();
        glm::vec2 otherSize = other->getSize();

        // Bottom to top
        if (std::abs(position.y + dockable->getSize().y - otherPos.y) < snapThreshold) {
            return DockPosition::Top;
        }
        // Top to bottom
        if (std::abs(position.y - (otherPos.y + otherSize.y)) < snapThreshold) {
            return DockPosition::Bottom;
        }
        // Right to left
        if (std::abs(position.x + dockable->getSize().x - otherPos.x) < snapThreshold) {
            return DockPosition::Left;
        }
        // Left to right
        if (std::abs(position.x - (otherPos.x + otherSize.x)) < snapThreshold) {
            return DockPosition::Right;
        }
    }

    return DockPosition::None;
}

} // namespace ui