#include "ui/UICanvas.h"
#include "ui/UILayout.h"
#include "ui/UIManager.h"
#include <spdlog/spdlog.h>
#include <algorithm>

namespace ui {

    std::unique_ptr<UICanvas> UICanvas::create(const std::string& styleType, int zIndex) {
        return std::unique_ptr<UICanvas>(new UICanvas(styleType, zIndex));
    }

    UICanvas::UICanvas(const std::string& styleType, int zIndex)
        : UIElement()
    {
        this->styleType_ = styleType;
        this->zIndex_ = zIndex;
        // Optionally, you can create a default layout here.
    }

    void UICanvas::setPosition(const glm::vec2& pos) {
        UIElement::setPosition(pos);
    }

    void UICanvas::render(IRenderer* renderer) {
        if (!renderer || !isDirty() || !isVisible()) return;
        UIManager::getInstance().queueForRender(this);
    }

    void UICanvas::doRender(IRenderer* renderer) {
        if (!renderer || !isDirty() || !isVisible()) return;
        const UITheme* theme = getEffectiveTheme();
        if (!theme) return;
        UIStyle style = *theme->getStyle(styleType_).get();
        renderer->drawRect(position_, size_, style.backgroundColor);
        for (auto& child : getMutableChildren()) {
            if (child)
                child->render(renderer);
        }
        markDirty();
    }

    bool UICanvas::handleInput(IMouseEvent* mouseEvent) {
        if (!mouseEvent || !isVisible()) return false;
        // Simplified input handling: pass event to children.
        if (!getMutableChildren().empty()) {
            for (auto it = getMutableChildren().rbegin(); it != getMutableChildren().rend(); ++it) {
                if ((*it)->hitTest(mouseEvent->getPosition()) && (*it)->handleInput(mouseEvent))
                    return true;
            }
        }
        return UIElement::handleInput(mouseEvent);
    }

    bool UICanvas::handleInput(IKeyboardEvent* keyboardEvent) {
        if (!keyboardEvent || !isVisible()) return false;
        for (auto& child : getMutableChildren()) {
            if (child && child->handleInput(keyboardEvent))
                return true;
        }
        return UIElement::handleInput(keyboardEvent);
    }

    bool UICanvas::handleInput(ITextInputEvent* textEvent) {
        if (!textEvent || !isVisible()) return false;
        for (auto& child : getMutableChildren()) {
            if (child && child->handleInput(textEvent))
                return true;
        }
        return UIElement::handleInput(textEvent);
    }

    void UICanvas::onStyleUpdate() {
        markDirty();
    }

    const std::vector<std::unique_ptr<UIElement>>& UICanvas::getChildren() const {
        return UIElement::getChildren();
    }

    void UICanvas::addChild(std::unique_ptr<UIElement> child) {
        if (child) {
            child->setParent(this);
            getMutableChildren().push_back(std::move(child));
            updateLayout();
            markDirty();
        }
    }

    void UICanvas::removeChild(const UIElement* child) {
        auto& children = getMutableChildren();
        children.erase(std::remove_if(children.begin(), children.end(),
            [child](const std::unique_ptr<UIElement>& ptr) {
                return ptr.get() == child;
            }),
            children.end());
        updateLayout();
        markDirty();
    }

    void UICanvas::addScrollbar(std::unique_ptr<UIScrollbar> scrollbar) {
        if (scrollbar) {
            scrollbar->setParent(this);
            scrollbars_.push_back(std::move(scrollbar));
            updateLayout();
            markDirty();
        }
    }

    void UICanvas::setScrollOffset(float xOffset, float yOffset) {
        scrollOffset_ = glm::vec2(xOffset, yOffset);
        markDirty();
    }

    void UICanvas::setLayout(std::unique_ptr<UILayout> layout) {
        layout_ = std::move(layout);
        updateLayout();
        markDirty();
    }

    void UICanvas::updateLayout() {
        if (layout_) {
            contentSize_ = layout_->arrange(this, getMutableChildren());
        }
    }

    glm::vec2 UICanvas::getCumulativeScrollOffset() const {
        return scrollOffset_;
    }

} // namespace ui
