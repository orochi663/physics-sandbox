#pragma once
#include "IInputTranslator.h"
#include "UITheme.h"
#include "UIElement.h"
#include <memory>
#include <vector>
#include <mutex>

namespace ui {

class UICanvas;
class UIDockable;

class UIManager {
public:
    static UIManager& getInstance();
    UIManager(const UIManager&) = delete;
    UIManager& operator=(const UIManager&) = delete;
    ~UIManager();

    // Input handling
    void processInput(void* rawEvent);

    // Theme management
    void setGlobalTheme(std::unique_ptr<UITheme> theme);
    void applyTheme();

    // Focus management
    void setFocusedElement(UIElement* element);
    UIElement* getFocusedElement() const { return focusedElement_; }
    void focusNext();
    void focusPrevious();
    void moveFocusToNextCanvas();

    // UI hierarchy management
    void addCanvas(std::unique_ptr<UICanvas> canvas);
    void removeCanvas(const UICanvas* canvas);
    const std::vector<std::unique_ptr<UICanvas>>& getCanvases() const { return canvases_; }

    // Update and render
    void update();
    void render(IRenderer* renderer);

    // Translator
    void setInputTranslator(std::unique_ptr<IInputTranslator> translator);

    // Docking management
    void handleDockableDragging(UIDockable* dockable, const glm::vec2& position);
    void handleDockableRelease(UIDockable* dockable);

private:
    UIManager();
    std::vector<std::unique_ptr<UICanvas>> canvases_;
    std::vector<UIDockable*> dockables_; // Track all dockable elements
    std::unique_ptr<IInputTranslator> translator_;
    std::unique_ptr<UITheme> globalTheme_;
    UIElement* focusedElement_{nullptr};
    std::mutex mutex_;

    bool dispatchMouseEvent(IMouseEvent* mouseEvent);
    bool dispatchKeyboardEvent(IKeyboardEvent* keyboardEvent);
    bool dispatchTextInputEvent(ITextInputEvent* textEvent);
    UIElement* findNextFocusable(UIElement* current, bool withinScope = true);
    UIElement* findPreviousFocusable(UIElement* current, bool withinScope = true);
    UICanvas* getCanvasForElement(const UIElement* element) const;
    void updateDockableSnapping(UIDockable* dockable, const glm::vec2& position);
    DockPosition checkDockableSnapping(UIDockable* dockable, const glm::vec2& position);
};

} // namespace ui