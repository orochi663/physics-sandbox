#pragma once
#include "UILayout.h"
#include <glm/glm.hpp>
#include <vector>

namespace ui {

class UIXYLayout : public UILayout {
public:
    UIXYLayout() = default;
    ~UIXYLayout() override = default;

    glm::vec2 arrange(UICanvas* canvas, std::vector<std::unique_ptr<UIElement>>& children) override;
    glm::vec2 calculateContentSize(const UICanvas* canvas,
                                   const std::vector<std::unique_ptr<UIElement>>& children) const override;
};

} // namespace ui