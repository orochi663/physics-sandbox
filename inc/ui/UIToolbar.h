#pragma once
#include "ui/UICanvas.h"
#include "ui/UIBoxLayout.h"
#include <memory>
#include <string>

namespace ui {

    class UIToolbar : public UICanvas {
    public:
        // Create a toolbar with a given orientation.
        static std::unique_ptr<UIToolbar> create(UIBoxLayout::Orientation orientation = UIBoxLayout::Orientation::Horizontal,
            const std::string& styleType = "toolbar",
            int zIndex = 0);
        void render(IRenderer* renderer) override;

        // Convenience method to add a button.
        void addButton(std::unique_ptr<UIButton> button);

    private:
        UIToolbar(UIBoxLayout::Orientation orientation, const std::string& styleType, int zIndex);
        std::unique_ptr<UIBoxLayout> layout_;
    };

} // namespace ui
