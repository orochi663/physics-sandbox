#include "UIXYLayout.h"
#include "UICanvas.h"

namespace ui {

glm::vec2 UIXYLayout::arrange(UICanvas* canvas, std::vector<std::unique_ptr<UIElement>>& children) {
    // Adjust positions based on padding
    for (auto& child : children) {
        if (child) {
            glm::vec2 pos = child->getPosition();
            pos.x += padding_.left + margin_.left;
            pos.y += padding_.top + margin_.top;
            child->setPosition(pos);
        }
    }
    return calculateContentSize(canvas, children);
}

glm::vec2 UIXYLayout::calculateContentSize(const UICanvas* canvas,
                                          const std::vector<std::unique_ptr<UIElement>>& children) const {
    if (children.empty()) {
        return glm::vec2(canvas->getSize().x - (padding_.left + padding_.right + margin_.left + margin_.right),
                         canvas->getSize().y - (padding_.top + padding_.bottom + margin_.top + margin_.bottom));
    }

    float maxX = padding_.left + margin_.left;
    float maxY = padding_.top + margin_.top;
    for (const auto& child : children) {
        if (child) {
            glm::vec2 childPos = child->getPosition();
            glm::vec2 childSize = child->getSize();
            maxX = std::max(maxX, childPos.x + childSize.x + padding_.right + margin_.right);
            maxY = std::max(maxY, childPos.y + childSize.y + padding_.bottom + margin_.bottom);
        }
    }
    return glm::vec2(maxX, maxY);
}

} // namespace ui