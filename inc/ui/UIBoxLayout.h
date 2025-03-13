#pragma once
#include "ui/UILayout.h"

namespace ui {

    class UIBoxLayout : public UILayout {
    public:
        enum class Orientation { Horizontal, Vertical };

        UIBoxLayout(Orientation orientation = Orientation::Horizontal, float spacing = 5.0f);

        // Use base class's virtual functions
        glm::vec2 arrange(UICanvas* canvas, std::vector<std::unique_ptr<UIElement>>& children) override;
        glm::vec2 calculateContentSize(const UICanvas* canvas,
            const std::vector<std::unique_ptr<UIElement>>& children) const override;

        // Added setOrientation to allow external changes.
        void setOrientation(Orientation orientation) { orientation_ = orientation; }

    private:
        Orientation orientation_;
        float spacing_;
    };

} // namespace ui
