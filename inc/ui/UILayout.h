#pragma once
#include "UIElement.h"
#include <glm/glm.hpp>
#include <vector>

namespace ui {

class UICanvas;

class UILayout {
public:
    virtual ~UILayout() = default;

    virtual glm::vec2 arrange(UICanvas* canvas, std::vector<std::unique_ptr<UIElement>>& children) = 0;
    virtual glm::vec2 calculateContentSize(const UICanvas* canvas,
                                           const std::vector<std::unique_ptr<UIElement>>& children) const = 0;

    void setPadding(float left, float right, float top, float bottom) {
        padding_ = {left, right, top, bottom};
    }
    void setMargin(float left, float right, float top, float bottom) {
        margin_ = {left, right, top, bottom};
    }

protected:
    struct Padding {
        float left{0.0f}, right{0.0f}, top{0.0f}, bottom{0.0f};
    };
    Padding padding_;
    Padding margin_;
};

} // namespace ui