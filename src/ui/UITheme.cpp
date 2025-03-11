#include "UITheme.h"

namespace ui {

void UITheme::setStyle(const std::string& type, UIStyle style) {
    styles_[type] = style;
}

UIStyle UITheme::getStyle(const std::string& type) const {
    auto it = styles_.find(type);
    return (it != styles_.end()) ? it->second : UIStyle();
}

void UITheme::clearStyles() {
    styles_.clear();
}

} // namespace ui