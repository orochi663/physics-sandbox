#pragma once
#include <string>

namespace ui {

class IFont {
public:
    virtual ~IFont() = default;
    virtual std::string getName() const = 0;
};

} // namespace ui