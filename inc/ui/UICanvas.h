#pragma once
#include "UIElement.h"
#include "UIScrollbar.h"
#include "UITheme.h"
#include "UILayout.h"
#include <memory>
#include <optional>
#include <vector>
#include <glm/glm.hpp>

namespace ui {

    class UICanvas : public UIElement {
    public:
        // Public factory function for creating a UICanvas.
        static std::unique_ptr<UICanvas> create(const std::string& styleType, int zIndex);

        ~UICanvas() override = default;

        // Override setPosition so derived classes can call it.
        virtual void setPosition(const glm::vec2& pos) override;

        // Rendering methods.
        void render(IRenderer* renderer) override;
        void doRender(IRenderer* renderer);

        bool handleInput(IMouseEvent* mouseEvent) override;
        bool handleInput(IKeyboardEvent* keyboardEvent) override;
        bool handleInput(ITextInputEvent* textEvent) override;
        void onStyleUpdate() override;
        virtual const std::vector<std::unique_ptr<UIElement>>& getChildren() const override;

        // Child management.
        virtual void addChild(std::unique_ptr<UIElement> child) override;
        virtual void removeChild(const UIElement* child) override;

        void addScrollbar(std::unique_ptr<UIScrollbar> scrollbar);
        void setScrollOffset(float xOffset, float yOffset);
        glm::vec2 getScrollOffset() const { return scrollOffset_; }

        // Provide computed content size.
        glm::vec2 getContentSize() const { return contentSize_; }

        // Layout.
        virtual void setLayout(std::unique_ptr<UILayout> layout);
        virtual void updateLayout();

        // Z-order, visibility, modality, and focus scoping.
        void setZIndex(int zIndex) { zIndex_ = zIndex; }
        int getZIndex() const { return zIndex_; }
        void setVisible(bool visible) { isVisible_ = visible; }
        bool isVisible() const { return isVisible_; }
        void setModal(bool modal) { isModal_ = modal; }
        bool isModal() const { return isModal_; }
        void setFocusScope(bool scope) { focusScope_ = scope; }
        bool isFocusScope() const { return focusScope_; }

        // Per-canvas theme override.
        void setThemeOverride(std::unique_ptr<UITheme> theme) { themeOverride_ = std::move(theme); }
        const UITheme* getEffectiveTheme() const { return themeOverride_ ? themeOverride_.get() : globalTheme_; }
        void setGlobalTheme(UITheme* theme) { globalTheme_ = theme; onStyleUpdate(); }

    protected:
        // Protected constructor so that clients must use the create() function.
        UICanvas(const std::string& styleType, int zIndex);

        // Expose protected access to the modifiable children vector.
        std::vector<std::unique_ptr<UIElement>>& getMutableChildren() { return children_; }

    private:
        // Unique_ptr holding scrollbars.
        std::vector<std::unique_ptr<UIScrollbar>> scrollbars_;
        std::unique_ptr<UILayout> layout_{ nullptr };
        std::unique_ptr<UITheme> themeOverride_;
        UITheme* globalTheme_{ nullptr };
        glm::vec2 scrollOffset_{ 0.0f, 0.0f };
        glm::vec2 contentSize_{ 0.0f, 0.0f };
        int zIndex_{ 0 };
        bool isVisible_{ true };
        bool isModal_{ false };
        bool focusScope_{ false };

        glm::vec2 getCumulativeScrollOffset() const;
    };

} // namespace ui
