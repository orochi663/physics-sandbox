#include "UILinearLayout.h"
#include "UICanvas.h"

namespace ui {

UILinearLayout::UILinearLayout(Orientation orientation, float spacing)
    : orientation_(orientation), spacing_(spacing) {}

glm::vec2 UILinearLayout::arrange(UICanvas* canvas, std::vector<std::unique_ptr<UIElement>>& children) {
    float offset = padding_.left + margin_.left;
    for (auto& child : children) {
        if (child) {
            if (orientation_ == Orientation::Horizontal) {
                child->setPosition(glm::vec2(offset, padding_.top + margin_.top));
                offset += child->getSize().x + spacing_;
            } else {
                child->setPosition(glm::vec2(padding_.left + margin_.left, offset));
                offset += child->getSize().y + spacing_;
            }
        }
    }
    return calculateContentSize(canvas, children);
}

glm::vec2 UILinearLayout::calculateContentSize(const UICanvas* canvas,
                                               const std::vector<std::unique_ptr<UIElement>>& children) const {
    if (children.empty()) {
        return glm::vec2(canvas->getSize().x - (padding_.left + padding_.right + margin_.left + margin_.right),
                         canvas->getSize().y - (padding_.top + padding_.bottom + margin_.top + margin_.bottom));
    }

    float totalWidth = padding_.left + padding_.right + margin_.left + margin_.right;
    float totalHeight = padding_.top + padding_.bottom + margin_.top + margin_.bottom;
    float maxWidth = 0.0f;
    float maxHeight = 0.0f;

    for (const auto& child : children) {
        if (child) {
            if (orientation_ == Orientation::Horizontal) {
                totalWidth += child->getSize().x + spacing_;
                maxHeight = std::max(maxHeight, child->getSize().y);
            } else {
                totalHeight += child->getSize().y + spacing_;
                maxWidth = std::max(maxWidth, child->getSize().x);
            }
        }
    }

    if (orientation_ == Orientation::Horizontal) {
        totalWidth = totalWidth > 0.0f ? totalWidth - spacing_ : 0.0f;
        return glm::vec2(totalWidth, maxHeight + padding_.top + padding_.bottom + margin_.top + margin_.bottom);
    } else {
        totalHeight = totalHeight > 0.0f ? totalHeight - spacing_ : 0.0f;
        return glm::vec2(maxWidth + padding_.left + padding_.right + margin_.left + margin_.right, totalHeight);
    }
}

} // namespace ui