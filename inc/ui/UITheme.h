#pragma once
#include "UIStyle.h"
#include <memory>
#include <string>
#include <unordered_map>

namespace ui {

class UITheme {
public:
    void setStyle(const std::string& type, UIStyle style);
    UIStyle getStyle(const std::string& type) const;
    void clearStyles();

private:
    std::unordered_map<std::string, UIStyle> styles_;
};

} // namespace ui