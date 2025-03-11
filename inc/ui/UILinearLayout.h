#pragma once
#include "UILayout.h"
#include <glm/glm.hpp>
#include <vector>

namespace ui {

class UILinearLayout : public UILayout {
public:
    enum class Orientation { Horizontal, Vertical };

    explicit UILinearLayout(Orientation orientation, float spacing = 5.0f);
    ~UILinearLayout() override = default;

    glm::vec2 arrange(UICanvas* canvas, std::vector<std::unique_ptr<UIElement>>& children) override;
    glm::vec2 calculateContentSize(const UICanvas* canvas,
                                   const std::vector<std::unique_ptr<UIElement>>& children) const override;

private:
    Orientation orientation_;
    float spacing_;
};

} // namespace ui