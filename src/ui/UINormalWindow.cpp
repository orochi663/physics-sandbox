#include "UINormalWindow.h"
#include "UILabel.h"
#include <spdlog/spdlog.h>

namespace ui {

std::unique_ptr<UINormalWindow> UINormalWindow::create(const std::string& title, const std::string& styleType, int zIndex) {
    return std::unique_ptr<UINormalWindow>(new UINormalWindow(title, styleType, zIndex));
}

UINormalWindow::UINormalWindow(const std::string& title, const std::string& styleType, int zIndex)
    : UICanvas(styleType, zIndex), title_(title) {
    titleLabel_ = std::make_unique<UILabel>(this);
    titleLabel_->setStyleType("windowTitle");
    titleLabel_->setText(title_);
    setTextAlignment(TextAlignment::Left); // Default to Left as per original design
}

void UINormalWindow::bringToFront() {
    int maxZIndex = zIndex_;
    for (const auto& canvas : UIManager::getInstance().getCanvases()) {
        if (canvas && canvas != this) {
            maxZIndex = std::max(maxZIndex, canvas->getZIndex());
        }
    }
    zIndex_ = maxZIndex + 1;
}

void UINormalWindow::render(IRenderer* renderer) {
    if (!renderer || !isVisible_ || !dirty_) return;

    const UITheme* theme = getEffectiveTheme();
    if (!theme) return;

    UIStyle style = theme->getStyle(styleType_);
    std::unordered_map<std::string, bool> states = {{"hovered", isHovered_}, {"pressed", isPressed_}};
    UIStyle effectiveStyle = style.computeEffectiveStyle(states);

    // Render background
    renderer->drawRect(position_, size_, effectiveStyle.backgroundColor);

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

    // Render content
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

bool UINormalWindow::handleInput(IMouseEvent* mouseEvent) {
    if (!mouseEvent || !isVisible_) return false;

    glm::vec2 pos = mouseEvent->getPosition();
    bool isInsideTitleBar = (pos.x >= position_.x && pos.x <= position_.x + size_.x &&
                             pos.y >= position_.y && pos.y <= position_.y + titleBarHeight_);

    if (mouseEvent->getType() == EventType::MousePress && mouseEvent->getButton() == MouseButton::Left) {
        if (isInsideTitleBar) {
            isDragging_ = true;
            dragStart_ = pos - position_;
            bringToFront();
            return true;
        }
    } else if (mouseEvent->getType() == EventType::MouseMove && isDragging_) {
        position_ = pos - dragStart_;
        return true;
    } else if (mouseEvent->getType() == EventType::MouseRelease) {
        isDragging_ = false;
        markDirty();
        return true;
    }

    return UICanvas::handleInput(mouseEvent);
}

} // namespace ui