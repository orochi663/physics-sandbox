#include "UIBoxLayout.h"
#include "UICanvas.h"

namespace ui {

UIBoxLayout::UIBoxLayout(Orientation orientation, float spacing)
    : orientation_(orientation), spacing_(spacing) {}

void UIBoxLayout::applyLayout(UICanvas* canvas) {
    if (!canvas) return;

    const auto& children = canvas->getChildren();
    if (children.empty()) return;

    glm::vec2 pos = canvas->getPosition();
    float offset = 0.0f;

    for (const auto& child : children) {
        if (!child) continue;

        child->setPosition(pos);
        if (orientation_ == Orientation::Horizontal) {
            pos.x += child->getSize().x + spacing_;
            offset = std::max(offset, child->getSize().y);
        } else {
            pos.y += child->getSize().y + spacing_;
            offset = std::max(offset, child->getSize().x);
        }
    }

    glm::vec2 canvasSize = canvas->getSize();
    if (orientation_ == Orientation::Horizontal) {
        canvasSize.x = pos.x - canvas->getPosition().x - spacing_;
        canvasSize.y = std::max(canvasSize.y, offset);
    } else {
        canvasSize.y = pos.y - canvas->getPosition().y - spacing_;
        canvasSize.x = std::max(canvasSize.x, offset);
    }
    canvas->setSize(canvasSize);
    canvas->markDirty();
}

} // namespace ui