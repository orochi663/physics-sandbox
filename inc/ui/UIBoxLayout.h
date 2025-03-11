#pragma once
#include "UILayout.h"

namespace ui {

class UIBoxLayout : public UILayout {
public:
    enum class Orientation { Horizontal, Vertical };

    UIBoxLayout(Orientation orientation = Orientation::Horizontal, float spacing = 5.0f);
    void applyLayout(UICanvas* canvas) override;

private:
    Orientation orientation_;
    float spacing_;
};

} // namespace ui