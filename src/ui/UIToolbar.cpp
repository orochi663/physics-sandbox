#include "ui/UIToolbar.h"
#include "ui/UIManager.h"
#include "ui/UIButton.h"
#include <spdlog/spdlog.h>

namespace ui {

    std::unique_ptr<UIToolbar> UIToolbar::create(UIBoxLayout::Orientation orientation,
        const std::string& styleType,
        int zIndex) {
        auto toolbar = std::unique_ptr<UIToolbar>(new UIToolbar(orientation, styleType, zIndex));
        UIManager::getInstance().addCanvas(std::move(toolbar));
        return toolbar;
    }

    UIToolbar::UIToolbar(UIBoxLayout::Orientation orientation, const std::string& styleType, int zIndex)
        : UICanvas(styleType, zIndex)
    {
        layout_ = std::make_unique<UIBoxLayout>(orientation);
        setLayout(std::move(layout_));
    }

    void UIToolbar::addButton(std::unique_ptr<UIButton> button) {
        if (button) {
            // Use inherited addChild() to add the button to the toolbar.
            addChild(std::move(button));
            markDirty();
        }
    }

    void UIToolbar::render(IRenderer* renderer) {
        // For simplicity, we let UICanvas handle the rendering.
        UICanvas::render(renderer);
    }

} // namespace ui
