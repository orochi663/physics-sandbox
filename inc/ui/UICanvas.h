#pragma once
#include "UIElement.h"
#include "UIScrollbar.h"
#include "UITheme.h"
#include <memory>
#include <optional>
#include <vector>

namespace ui {

class UILayout;

class UICanvas : public UIElement {
    friend class CanvasFactory;
protected:
    UICanvas(const std::string& styleType, int zIndex);
public:
    ~UICanvas() override = default;

    void render(IRenderer* renderer) override;
    bool handleInput(IMouseEvent* mouseEvent) override;
    bool handleInput(IKeyboardEvent* keyboardEvent) override;
    bool handleInput(ITextInputEvent* textEvent) override;
    void onStyleUpdate() override;
    const std::vector<std::unique_ptr<UIElement>>& getChildren() const override;

    void addChild(std::unique_ptr<UIElement> child) override;
    void removeChild(const UIElement* child) override;

    void addScrollbar(std::unique_ptr<UIScrollbar> scrollbar);
    void setScrollOffset(float xOffset, float yOffset);
    glm::vec2 getScrollOffset() const { return scrollOffset_; }

    void setLayout(std::unique_ptr<UILayout> layout);
    void updateLayout();

    // Z-order, visibility, modality, and focus scoping
    void setZIndex(int zIndex) { zIndex_ = zIndex; }
    int getZIndex() const { return zIndex_; }
    void setVisible(bool visible) { isVisible_ = visible; }
    bool isVisible() const { return isVisible_; }
    void setModal(bool modal) { isModal_ = modal; }
    bool isModal() const { return isModal_; }
    void setFocusScope(bool scope) { focusScope_ = scope; }
    bool isFocusScope() const { return focusScope_; }

    // Per-canvas theme override
    void setThemeOverride(std::unique_ptr<UITheme> theme) { themeOverride_ = std::move(theme); }
    const UITheme* getEffectiveTheme() const { return themeOverride_ ? themeOverride_.get() : globalTheme_; }
    void setGlobalTheme(UITheme* theme) { globalTheme_ = theme; onStyleUpdate(); }

private:
    std::vector<std::unique_ptr<UIElement>> children_;
    std::vector<std::unique_ptr<UIScrollbar>> scrollbars_;
    std::unique_ptr<UILayout> layout_{nullptr};
    std::unique_ptr<UITheme> themeOverride_;
    UITheme* globalTheme_{nullptr};
    glm::vec2 scrollOffset_{0.0f, 0.0f};
    glm::vec2 contentSize_{0.0f, 0.0f};
    int zIndex_{0};
    bool isVisible_{true};
    bool isModal_{false};
    bool focusScope_{false};

    glm::vec2 getCumulativeScrollOffset() const;
};

} // namespace ui