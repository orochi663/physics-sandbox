#include "ui/UIBoxLayout.h"
#include "ui/UICanvas.h"
#include <algorithm>
#include <glm/glm.hpp>

namespace ui {

    UIBoxLayout::UIBoxLayout(Orientation orientation, float spacing)
        : orientation_(orientation), spacing_(spacing) {
    }

    glm::vec2 UIBoxLayout::arrange(UICanvas* canvas, std::vector<std::unique_ptr<UIElement>>& children) {
        if (!canvas || children.empty()) return canvas->getSize();

        // Use the canvas position as the starting point.
        glm::vec2 pos = canvas->getPosition();
        float maxSecondary = 0.0f;

        // Arrange each child sequentially.
        for (const auto& child : children) {
            if (!child) continue;
            child->setPosition(pos);
            if (orientation_ == Orientation::Horizontal) {
                pos.x += child->getSize().x + spacing_;
                maxSecondary = std::max(maxSecondary, child->getSize().y);
            }
            else { // Vertical
                pos.y += child->getSize().y + spacing_;
                maxSecondary = std::max(maxSecondary, child->getSize().x);
            }
        }

        // Calculate the new canvas size based on arranged children.
        glm::vec2 canvasSize = canvas->getSize();
        if (orientation_ == Orientation::Horizontal) {
            canvasSize.x = pos.x - canvas->getPosition().x - spacing_;
            canvasSize.y = std::max(canvasSize.y, maxSecondary);
        }
        else {
            canvasSize.y = pos.y - canvas->getPosition().y - spacing_;
            canvasSize.x = std::max(canvasSize.x, maxSecondary);
        }
        canvas->setSize(canvasSize);
        canvas->markDirty();
        return canvasSize;
    }

    glm::vec2 UIBoxLayout::calculateContentSize(const UICanvas* canvas,
        const std::vector<std::unique_ptr<UIElement>>& children) const {
        if (!canvas || children.empty()) return canvas->getSize();

        float totalPrimary = 0.0f;
        float maxSecondary = 0.0f;
        for (const auto& child : children) {
            if (!child) continue;
            if (orientation_ == Orientation::Horizontal) {
                totalPrimary += child->getSize().x + spacing_;
                maxSecondary = std::max(maxSecondary, child->getSize().y);
            }
            else {
                totalPrimary += child->getSize().y + spacing_;
                maxSecondary = std::max(maxSecondary, child->getSize().x);
            }
        }
        if (orientation_ == Orientation::Horizontal) {
            totalPrimary -= spacing_; // Remove extra spacing after last element.
            return glm::vec2(totalPrimary, maxSecondary);
        }
        else {
            totalPrimary -= spacing_;
            return glm::vec2(maxSecondary, totalPrimary);
        }
    }

} // namespace ui
