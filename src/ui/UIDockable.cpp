#include "UIDockable.h"
#include "UILabel.h"
#include "UIManager.h"
#include <spdlog/spdlog.h>

namespace ui {

std::unique_ptr<UIDockable> UIDockable::create(const std::string& title, const std::string& styleType, int zIndex) {
    return std::unique_ptr<UIDockable>(new UIDockable(title, styleType, zIndex));
}

UIDockable::UIDockable(const std::string& title, const std::string& styleType, int zIndex)
    : UICanvas(styleType, zIndex), title_(title) {
    titleLabel_ = std::make_unique<UILabel>(this);
    titleLabel_->setText(title_);
    titleLabel_->setStyleType("dockableTitle");
    setTextAlignment(TextAlignment::Left); // Default to Left as per original design
    size_ = glm::vec2(200.0f, 150.0f);
    UIManager::getInstance().addCanvas(this); // Register with UIManager
}

void UIDockable::setDockPosition(DockPosition position) {
    dockPosition_ = position;
    updateDocking();
    markDirty();
}

void UIDockable::updateDocking() {
    IRenderer* renderer = nullptr; // Assume access to renderer (e.g., via UICanvas or UIManager)
    glm::vec2 screenSize = (renderer) ? renderer->getScreenSize() : glm::vec2(1280.0f, 720.0f);

    switch (dockPosition_) {
        case DockPosition::Top:
            position_ = glm::vec2(0.0f, 0.0f);
            size_.x = screenSize.x;
            break;
        case DockPosition::Bottom:
            position_ = glm::vec2(0.0f, screenSize.y - size_.y);
            size_.x = screenSize.x;
            break;
        case DockPosition::Left:
            position_ = glm::vec2(0.0f, 0.0f);
            size_.y = screenSize.y;
            break;
        case DockPosition::Right:
            position_ = glm::vec2(screenSize.x - size_.x, 0.0f);
            size_.y = screenSize.y;
            break;
        case DockPosition::None:
            // Floating; no position adjustment
            break;
    }
}

void UIDockable::setPosition(const glm::vec2& pos) {
    UICanvas::setPosition(pos); // Call base class method
    if (dockPosition_ == DockPosition::None) {
        markDirty();
    }
}

void UIDockable::render(IRenderer* renderer) {
    if (!renderer || !isVisible_ || !dirty_) return;

    const UITheme* theme = getEffectiveTheme();
    if (!theme) return;

    UIStyle style = theme->getStyle(styleType_);
    std::unordered_map<std::string, bool> states = {{"hovered", isHovered_}, {"pressed", isPressed_}};
    UIStyle effectiveStyle = style.computeEffectiveStyle(states);

    // Render title bar
    glm::vec2 titleBarPos = position_;
    glm::vec2 titleBarSize(size_.x, titleBarHeight_);
    renderer->drawRect(titleBarPos, titleBarSize, effectiveStyle.backgroundColor);

    // Position title label based on text alignment
    glm::vec2 labelPos = titleBarPos;
    switch (getTextAlignment()) {
        case TextAlignment::Left:
            labelPos.x += 5.0f;
            break;
        case TextAlignment::Right:
            labelPos.x += titleBarSize.x - titleLabel_->getSize().x - 5.0f;
            break;
        default:
            labelPos.x += 5.0f; // Default to Left
            break;
    }
    titleLabel_->setPosition(labelPos);
    titleLabel_->render(renderer);

    // Adjust content area
    glm::vec2 contentPos = position_ + glm::vec2(0.0f, titleBarHeight_);
    glm::vec2 contentSize(size_.x, size_.y - titleBarHeight_);
    renderer->setClipRect(contentPos, contentSize);

    glm::vec2 cumulativeOffset = getCumulativeScrollOffset();
    glm::vec2 adjustedPos = contentPos - cumulativeOffset;

    for (const auto& child : children_) {
        if (child) {
            glm::vec2 childPos = child->getPosition() - cumulativeOffset;
            child->setPosition(childPos);
            child->render(renderer);
            child->setPosition(childPos + cumulativeOffset);
        }
    }

    renderer->resetClipRect();
    dirty_ = false;
}

bool UIDockable::handleInput(IMouseEvent* mouseEvent) {
    if (!mouseEvent || !isVisible_) return false;

    glm::vec2 pos = mouseEvent->getPosition();
    bool isInsideTitleBar = (pos.x >= position_.x && pos.x <= position_.x + size_.x &&
                             pos.y >= position_.y && pos.y <= position_.y + titleBarHeight_);

    if (mouseEvent->getType() == EventType::MousePress && mouseEvent->getButton() == MouseButton::Left && isInsideTitleBar) {
        isDragging_ = true;
        dragStart_ = pos - position_;
        if (dockPosition_ != DockPosition::None) {
            dockPosition_ = DockPosition::None; // Undock
            size_ = glm::vec2(200.0f, 150.0f);  // Reset size
            UIManager::getInstance().handleDockableDragging(this, position_);
        }
        return true;
    } else if (mouseEvent->getType() == EventType::MouseMove && isDragging_) {
        position_ = pos - dragStart_;
        UIManager::getInstance().handleDockableDragging(this, position_);
        return true;
    } else if (mouseEvent->getType() == EventType::MouseRelease && isDragging_) {
        isDragging_ = false;
        UIManager::getInstance().handleDockableRelease(this);
        markDirty();
        return true;
    }

    return UICanvas::handleInput(mouseEvent);
}

} // namespace ui