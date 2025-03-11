#pragma once
#include "UILayout.h"
#include <glm/glm.hpp>
#include <vector>

namespace ui {

class UIGridLayout : public UILayout {
public:
    UIGridLayout(int columns, float spacing = 5.0f);
    ~UIGridLayout() override = default;

    glm::vec2 arrange(UICanvas* canvas, std::vector<std::unique_ptr<UIElement>>& children) override;
    glm::vec2 calculateContentSize(const UICanvas* canvas,
                                   const std::vector<std::unique_ptr<UIElement>>& children) const override;

private:
    int columns_;
    float spacing_;
};

} // namespace ui