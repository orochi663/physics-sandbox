#include "ui/UINormalWindow.h"
#include "ui/UILabel.h"
#include "ui/UIManager.h"
#include <spdlog/spdlog.h>
#include <glm/glm.hpp>
#include <algorithm>

namespace ui {

    std::unique_ptr<UINormalWindow> UINormalWindow::create(const std::string& title, const std::string& styleType, int zIndex) {
        return std::unique_ptr<UINormalWindow>(new UINormalWindow(title, styleType, zIndex));
    }

    UINormalWindow::UINormalWindow(const std::string& title, const std::string& styleType, int zIndex)
        : UICanvas(styleType, zIndex), title_(title)
    {
        titleLabel_ = UILabel::create(title_);
        titleLabel_->setStyleType("windowTitle");
        titleLabel_->setParent(this);
        setTextAlignment(TextAlignment::Left);
    }

    void UINormalWindow::bringToFront() {
        int maxZIndex = getZIndex();
        for (const auto& canvas : UIManager::getInstance().getCanvases()) {
            if (canvas && canvas.get() != this) {
                maxZIndex = std::max(maxZIndex, canvas->getZIndex());
            }
        }
        setZIndex(maxZIndex + 1);
    }

    void UINormalWindow::render(IRenderer* renderer) {
        if (!renderer || !isDirty() || !isVisible()) return;
        drawBackground(renderer);
        glm::vec2 titleBarPos = position_;
        glm::vec2 titleBarSize = glm::vec2(size_.x, 30.0f);
        renderer->drawRect(titleBarPos, titleBarSize, UIManager::getInstance().getEffectiveTheme()->getStyle("windowTitle").backgroundColor);
        titleLabel_->setPosition(titleBarPos + glm::vec2(5.0f, 5.0f));
        titleLabel_->render(renderer);
        UICanvas::render(renderer);
    }

    bool UINormalWindow::handleInput(IMouseEvent* mouseEvent) {
        if (!mouseEvent || !isVisible()) return false;
        glm::vec2 pos = mouseEvent->getPosition();
        bool isInsideTitleBar = (pos.x >= position_.x && pos.x <= position_.x + size_.x &&
            pos.y >= position_.y && pos.y <= position_.y + 30.0f);
        if (mouseEvent->getType() == EventType::MousePress && mouseEvent->getButton() == MouseButton::Left && isInsideTitleBar) {
            isDragging_ = true;
            dragStart_ = pos - position_;
            bringToFront();
            return true;
        }
        else if (mouseEvent->getType() == EventType::MouseMove && isDragging_) {
            setPosition(pos - dragStart_);
            return true;
        }
        else if (mouseEvent->getType() == EventType::MouseRelease && isDragging_) {
            isDragging_ = false;
            markDirty();
            return true;
        }
        return UICanvas::handleInput(mouseEvent);
    }

    void UINormalWindow::setTitle(const std::string& title) {
        title_ = title;
        if (titleLabel_) {
            titleLabel_->setText(title);
        }
        markDirty();
    }

    void UINormalWindow::onStyleUpdate() {
        if (titleLabel_) {
            titleLabel_->onStyleUpdate();
        }
        markDirty();
    }

} // namespace ui
