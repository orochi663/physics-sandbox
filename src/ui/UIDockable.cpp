#include "ui/UIDockable.h"
#include "ui/UILabel.h"
#include "ui/UIManager.h"
#include "ui/UITheme.h"
#include <spdlog/spdlog.h>
#include <glm/glm.hpp>
#include <algorithm>

namespace ui {

    std::unique_ptr<UIDockable> UIDockable::create(const std::string& title, const std::string& styleType, int zIndex) {
        return std::unique_ptr<UIDockable>(new UIDockable(title, styleType, zIndex));
    }

    UIDockable::UIDockable(const std::string& title, const std::string& styleType, int zIndex)
        : UICanvas(styleType, zIndex), title_(title)
    {
        titleLabel_ = UILabel::create(title_);
		titleLabel_->setParent(this);
        titleLabel_->setStyleType("dockableTitle");
        setTextAlignment(TextAlignment::Left);
    }

    void UIDockable::setDockPosition(DockPosition position) {
        dockPosition_ = position;
        updateDocking();
        markDirty();
    }

    void UIDockable::updateDocking() {
        // Remove dependency on UIManager::getScreenSize() by using a fallback.
        glm::vec2 screenSize(1280.0f, 720.0f);
        if (dockPosition_ == DockPosition::Top) {
            setPosition(glm::vec2(0.0f, 0.0f));
        }
        else if (dockPosition_ == DockPosition::Bottom) {
            setPosition(glm::vec2(0.0f, screenSize.y - size_.y));
        }
        // Left and Right can be added similarly.
    }

    void UIDockable::setTitle(const std::string& title) {
        title_ = title;
        if (titleLabel_) {
            titleLabel_->setText(title);
        }
        markDirty();
    }

    void UIDockable::setPosition(const glm::vec2& pos) {
        UICanvas::setPosition(pos);
        if (dockPosition_ == DockPosition::None) {
            markDirty();
        }
    }

    void UIDockable::render(IRenderer* renderer) {
        if (!renderer || !isDirty() || !isVisible()) return;
        drawBackground(renderer);
        glm::vec2 titleBarPos = position_;
        glm::vec2 titleBarSize = glm::vec2(size_.x, titleBarHeight_);
        // Use getEffectiveTheme()->getStyle(styleType_) with pointer dereference.
        const UITheme* theme = getEffectiveTheme();
        if (!theme) return;
        std::shared_ptr<UIStyle> style = theme->getStyle(styleType_);
        renderer->drawRect(titleBarPos, titleBarSize, style->backgroundColor);
        glm::vec2 labelPos = titleBarPos + glm::vec2(5.0f, 5.0f);
        titleLabel_->setPosition(labelPos);
        titleLabel_->render(renderer);
        glm::vec2 contentPos = position_ + glm::vec2(0.0f, titleBarHeight_);
        glm::vec2 contentSize = size_ - glm::vec2(0.0f, titleBarHeight_);
        renderer->setClipRect(contentPos, contentSize);
        for (const auto& child : getChildren()) {
            if (child) child->render(renderer);
        }
        renderer->resetClipRect();
        markDirty();
    }

    bool UIDockable::handleInput(IMouseEvent* mouseEvent) {
        if (!mouseEvent || !isVisible()) return false;
        glm::vec2 pos = mouseEvent->getPosition();
        bool isInsideTitleBar = (pos.x >= position_.x && pos.x <= position_.x + size_.x &&
            pos.y >= position_.y && pos.y <= position_.y + titleBarHeight_);
        if (mouseEvent->getType() == EventType::MousePress && mouseEvent->getButton() == MouseButton::Left && isInsideTitleBar) {
            isDragging_ = true;
            dragStart_ = pos - position_;
            if (dockPosition_ != DockPosition::None) {
                dockPosition_ = DockPosition::None;
            }
            return true;
        }
        else if (mouseEvent->getType() == EventType::MouseMove && isDragging_) {
            setPosition(pos - dragStart_);
            return true;
        }
        else if (mouseEvent->getType() == EventType::MouseRelease && isDragging_) {
            isDragging_ = false;
            UIManager::getInstance().handleDockableRelease(this);
            markDirty();
            return true;
        }
        return UICanvas::handleInput(mouseEvent);
    }

    void UIDockable::onStyleUpdate() {
        if (titleLabel_) {
            titleLabel_->onStyleUpdate();
        }
        markDirty();
    }

} // namespace ui
