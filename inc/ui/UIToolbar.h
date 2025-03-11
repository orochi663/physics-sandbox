#pragma once
#include "UICanvas.h"
#include "UIBoxLayout.h"
#include <memory>

namespace ui {

enum class Orientation {
    Horizontal,
    Vertical
};

class UIToolbar : public UICanvas {
public:
    static std::unique_ptr<UIToolbar> create(Orientation orientation = Orientation::Horizontal);
    void render(IRenderer* renderer) override;
    void addChild(std::unique_ptr<UIElement> child) override;
    void setOrientation(Orientation orientation) { orientation_ = orientation; layout_->setOrientation(orientation); }

private:
    UIToolbar(Orientation orientation);
    Orientation orientation_;
    std::unique_ptr<UIBoxLayout> layout_;
};

} // namespace ui