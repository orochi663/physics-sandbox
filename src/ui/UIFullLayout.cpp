#include "UIFullLayout.h"
#include "UICanvas.h"
#include <spdlog/spdlog.h>

namespace ui {

glm::vec2 UIFullLayout::arrange(UICanvas* canvas, std::vector<std::unique_ptr<UIElement>>& children) {
    if (children.empty()) {
        spdlog::warn("UIFullLayout: No children to arrange");
        currentChild = nullptr;
        return canvas->getSize();
    }
    if (children.size() > 1) {
        spdlog::warn("UIFullLayout: More than one child detected; removing previous children");
        // Remove all children except the first
        while (children.size() > 1) {
            children.pop_back();
        }
        if (currentChild && currentChild != children[0].get()) {
            // Remove the previous child if it exists and differs
            for (auto it = canvas->getChildren().begin(); it != canvas->getChildren().end(); ++it) {
                if (it->get() == currentChild) {
                    canvas->removeChild(currentChild);
                    break;
                }
            }
        }
    }

    auto& child = children[0];
    if (child) {
        currentChild = child.get();
        glm::vec2 paddedSize = canvas->getSize() - glm::vec2(padding_.left + padding_.right, padding_.top + padding_.bottom);
        child->setPosition(glm::vec2(padding_.left + margin_.left, padding_.top + margin_.top));
        child->setSize(paddedSize);
    }
    return calculateContentSize(canvas, children);
}

glm::vec2 UIFullLayout::calculateContentSize(const UICanvas* canvas,
                                             const std::vector<std::unique_ptr<UIElement>>& children) const {
    if (children.empty()) {
        return canvas->getSize();
    }
    return children[0]->getSize() + glm::vec2(padding_.left + padding_.right + margin_.left + margin_.right,
                                              padding_.top + padding_.bottom + margin_.top + margin_.bottom);
}

} // namespace ui