#include "UIToolbar.h"

namespace ui {

std::unique_ptr<UIToolbar> UIToolbar::create(UIBoxLayout::Orientation orientation, const std::string& styleType, int zIndex) {
    return std::unique_ptr<UIToolbar>(new UIToolbar(orientation, styleType, zIndex));
}

UIToolbar::UIToolbar(UIBoxLayout::Orientation orientation, const std::string& styleType, int zIndex)
    : UICanvas(styleType, zIndex) {
    layout_ = std::make_unique<UIBoxLayout>(orientation);
}

void UIToolbar::setOrientation(UIBoxLayout::Orientation orientation) {
    if (auto* boxLayout = dynamic_cast<UIBoxLayout*>(layout_.get())) {
        boxLayout->setOrientation(orientation);
        updateLayout();
        markDirty();
    }
}

} // namespace ui