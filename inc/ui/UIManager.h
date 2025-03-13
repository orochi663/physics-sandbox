#ifndef UI_MANAGER_H
#define UI_MANAGER_H

#include "IRenderer.h"
#include "IInputTranslator.h"
#include "UITheme.h"
#include "UICanvas.h"
#include "UIDockable.h"
#include "UIElement.h"
#include "UIEventBus.h" // Added for event publishing
#include <memory>
#include <vector>
#include <mutex>
#include <queue>
#include <chrono>
#include <coroutine>
#include <thread>
#include <condition_variable>

namespace ui {

enum class DockPosition {
    None,
    Top,
    Bottom,
    Left,
    Right
};

struct CoroutineSchedule {
    std::chrono::steady_clock::time_point resumeTime;
    std::coroutine_handle<> handle;
    bool operator>(const CoroutineSchedule& other) const { return resumeTime > other.resumeTime; }
};

class UIManager {
public:
    static UIManager& getInstance();

    void processInput(void* rawEvent);
    void setGlobalTheme(std::unique_ptr<UITheme> theme);
    void setFocusedElement(UIElement* element);
    void focusNext();
    void focusPrevious();
    void moveFocusToNextCanvas();
    void addCanvas(std::unique_ptr<UICanvas> canvas);
    void removeCanvas(const UICanvas* canvas);
    void scheduleCoroutine(float seconds, std::coroutine_handle<> handle);
    void update();
    void render(IRenderer* renderer);
    void queueForRender(UICanvas* canvas);
    void setInputTranslator(std::unique_ptr<IInputTranslator> translator);
    void handleDockableDragging(UIDockable* dockable, const glm::vec2& position);
    void handleDockableRelease(UIDockable* dockable);

private:
    UIManager();
    ~UIManager();
    UIManager(const UIManager&) = delete;
    UIManager& operator=(const UIManager&) = delete;

    // Core UI management
    std::vector<std::unique_ptr<UICanvas>> canvases_;
    std::vector<UIDockable*> dockables_;
    std::unique_ptr<UITheme> globalTheme_;
    std::unique_ptr<IInputTranslator> translator_;
    UIElement* focusedElement_ = nullptr;
    mutable std::mutex mutex_;

    // Coroutine scheduling
    std::priority_queue<CoroutineSchedule, std::vector<CoroutineSchedule>, std::greater<CoroutineSchedule>> coroutineQueue_;
    std::jthread coroutineThread_;
    std::condition_variable coroutineCv_;
    bool coroutineThreadRunning_ = true;

    // Render thread management
    std::jthread renderThread_;
    std::mutex renderMutex_;
    std::condition_variable renderCv_;
    std::vector<UICanvas*> renderQueue_;
    bool renderThreadRunning_ = true;
    IRenderer* renderer_ = nullptr;

    // Private helper methods
    void applyTheme();
    void renderLoop();
    void coroutineLoop();
    UIElement* findNextFocusable(UIElement* current, bool withinScope = true);
    UIElement* findPreviousFocusable(UIElement* current, bool withinScope = true);
    UICanvas* getCanvasForElement(const UIElement* element) const;
    void updateDockableSnapping(UIDockable* dockable, const glm::vec2& position);
    DockPosition checkDockableSnapping(UIDockable* dockable, const glm::vec2& position, const glm::vec2& screenSize = glm::vec2(1280.0f, 720.0f));
};

inline void UIManager::queueForRender(UICanvas* canvas) {
    std::lock_guard<std::mutex> lock(renderMutex_);
    renderQueue_.push_back(canvas);
    renderCv_.notify_one();
}

} // namespace ui

#endif // UI_MANAGER_H