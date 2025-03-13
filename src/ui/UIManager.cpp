#include "ui/UIManager.h"
#include "ui/UICanvas.h"
#include "ui/UIDockable.h"
#include "ui/UIElement.h"
#include "ui/UIEventBus.h" // Added for event publishing
#include <algorithm>
#include <spdlog/spdlog.h>
#include <chrono>
#include <queue>
#include <coroutine>
#include <thread>
#include <condition_variable>

namespace ui {

UIManager& UIManager::getInstance() {
    static UIManager instance;
    return instance;
}

UIManager::UIManager() 
    : renderThread_([this]() { renderLoop(); }),
      coroutineThread_([this]() { coroutineLoop(); }) {
}

UIManager::~UIManager() {
    {
        std::lock_guard<std::mutex> lock(mutex_);
        renderThreadRunning_ = false;
        coroutineThreadRunning_ = false;
    }
    renderCv_.notify_all();
    coroutineCv_.notify_all();
    renderThread_.join();
    coroutineThread_.join();
    std::lock_guard<std::mutex> lock(mutex_);
    canvases_.clear();
    dockables_.clear();
    while (!coroutineQueue_.empty()) {
        auto handle = coroutineQueue_.top().handle;
        if (handle && !handle.done()) handle.destroy();
        coroutineQueue_.pop();
    }
}

void UIManager::processInput(void* rawEvent) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!translator_) {
        spdlog::warn("No input translator set in UIManager");
        return;
    }

    std::unique_ptr<IInputEvent> event = translator_->translate(rawEvent);
    if (!event) return;

    UIEventBus& eventBus = UIEventBus::getInstance();
    switch (event->getType()) {
        case EventType::MouseMove:
            eventBus.publish("MouseMove", nullptr, event->getType());
            break;
        case EventType::MousePress:
            eventBus.publish("MousePress", nullptr, event->getType());
            break;
        case EventType::MouseRelease:
            eventBus.publish("MouseRelease", nullptr, event->getType());
            break;
        case EventType::KeyPress:
            eventBus.publish("KeyPress", focusedElement_, event->getType());
            break;
        case EventType::KeyRelease:
            eventBus.publish("KeyRelease", focusedElement_, event->getType());
            break;
        case EventType::TextInput:
            eventBus.publish("TextInput", focusedElement_, event->getType());
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
            if (canvas) canvas->applyTheme(globalTheme_.get());
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

void UIManager::scheduleCoroutine(float seconds, std::coroutine_handle<> handle) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!handle) {
        spdlog::warn("Attempted to schedule null coroutine handle");
        return;
    }
    auto resumeTime = std::chrono::steady_clock::now() + std::chrono::duration_cast<std::chrono::steady_clock::duration>(
        std::chrono::duration<float>(seconds));
    coroutineQueue_.push({resumeTime, handle});
    coroutineCv_.notify_one();
}

void UIManager::update() {
    std::lock_guard<std::mutex> lock(mutex_);
    for (auto& canvas : canvases_) {
        if (canvas) canvas->update();
    }

    std::vector<UICanvas*> dirtyCanvases;
    for (auto& canvas : canvases_) {
        if (canvas && canvas->isDirty()) {
            dirtyCanvases.push_back(canvas.get());
        }
    }
    if (!dirtyCanvases.empty()) {
        std::lock_guard<std::mutex> renderLock(renderMutex_);
        renderQueue_.insert(renderQueue_.end(), dirtyCanvases.begin(), dirtyCanvases.end());
        renderCv_.notify_one();
    }
}

void UIManager::render(IRenderer* renderer) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!renderer) return;
    renderer_ = renderer;
}

void UIManager::renderLoop() {
    while (renderThreadRunning_) {
        std::vector<UICanvas*> canvasesToRender;
        {
            std::unique_lock<std::mutex> lock(renderMutex_);
            renderCv_.wait(lock, [this]() { return !renderQueue_.empty() || !renderThreadRunning_; });
            if (!renderThreadRunning_) break;
            canvasesToRender.swap(renderQueue_);
        }

        if (renderer_) {
            std::sort(canvasesToRender.begin(), canvasesToRender.end(),
                      [](const UICanvas* a, const UICanvas* b) { return a->getZIndex() < b->getZIndex(); });
            for (auto* canvas : canvasesToRender) {
                if (canvas && canvas->isVisible()) {
                    canvas->doRender(renderer_);
                }
            }
        }
    }
}

void UIManager::coroutineLoop() {
    while (coroutineThreadRunning_) {
        std::unique_lock<std::mutex> lock(mutex_);
        if (coroutineQueue_.empty()) {
            coroutineCv_.wait(lock, [this]() { return !coroutineQueue_.empty() || !coroutineThreadRunning_; });
            if (!coroutineThreadRunning_) break;
        }

        auto now = std::chrono::steady_clock::now();
        while (!coroutineQueue_.empty() && coroutineQueue_.top().resumeTime <= now) {
            auto [resumeTime, handle] = coroutineQueue_.top();
            coroutineQueue_.pop();
            lock.unlock();
            if (handle && !handle.done()) {
                handle.resume();
            } else if (handle) {
                spdlog::warn("Attempted to resume completed coroutine");
                handle.destroy();
            }
            lock.lock();
        }

        if (!coroutineQueue_.empty()) {
            auto nextWake = coroutineQueue_.top().resumeTime;
            lock.unlock();
            std::this_thread::sleep_until(nextWake);
            lock.lock();
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

    glm::vec2 originalPos = dockable->getPosition();
    dockable->setPosition(position);

    IRenderer* renderer = renderer_;
    glm::vec2 screenSize = (renderer) ? renderer->getScreenSize() : glm::vec2(1280.0f, 720.0f);

    DockPosition newPosition = checkDockableSnapping(dockable, position, screenSize);
    if (newPosition != DockPosition::None) {
        dockable->setDockPosition(newPosition);
    } else {
        dockable->setDockPosition(DockPosition::None);
    }

    if (dockable->getDockPosition() == DockPosition::None) {
        dockable->setPosition(originalPos);
    }
}

DockPosition UIManager::checkDockableSnapping(UIDockable* dockable, const glm::vec2& position, const glm::vec2& screenSize) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!dockable) return DockPosition::None;

    const float snapThreshold = 20.0f;

    if (position.y < snapThreshold) return DockPosition::Top;
    if (position.y + dockable->getSize().y > screenSize.y - snapThreshold) return DockPosition::Bottom;
    if (position.x < snapThreshold) return DockPosition::Left;
    if (position.x + dockable->getSize().x > screenSize.x - snapThreshold) return DockPosition::Right;

    for (UIDockable* other : dockables_) {
        if (other == dockable || other->getDockPosition() == DockPosition::None) continue;

        glm::vec2 otherPos = other->getPosition();
        glm::vec2 otherSize = other->getSize();

        if (std::abs(position.y + dockable->getSize().y - otherPos.y) < snapThreshold) {
            return DockPosition::Top;
        }
        if (std::abs(position.y - (otherPos.y + otherSize.y)) < snapThreshold) {
            return DockPosition::Bottom;
        }
        if (std::abs(position.x + dockable->getSize().x - otherPos.x) < snapThreshold) {
            return DockPosition::Left;
        }
        if (std::abs(position.x - (otherPos.x + otherSize.x)) < snapThreshold) {
            return DockPosition::Right;
        }
    }

    return DockPosition::None;
}

} // namespace ui