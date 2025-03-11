#pragma once
#include "UILayout.h"
#include <glm/glm.hpp>
#include <vector>

namespace ui {

class UIFullLayout : public UILayout {
public:
    UIFullLayout() = default;
    ~UIFullLayout() override = default;

    glm::vec2 arrange(UICanvas* canvas, std::vector<std::unique_ptr<UIElement>>& children) override;
    glm::vec2 calculateContentSize(const UICanvas* canvas,
                                   const std::vector<std::unique_ptr<UIElement>>& children) const override;

private:
    UIElement* currentChild{nullptr}; // Track the current child
};

} // namespace ui