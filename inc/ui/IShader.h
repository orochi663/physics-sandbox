#pragma once
#include <string>

namespace ui {

class IShader {
public:
    virtual ~IShader() = default;

    virtual std::string getName() const = 0;
    virtual std::string getType() const = 0; // e.g., "vertex", "fragment"
    // Additional methods (e.g., getHandle()) can be added later
};

} // namespace ui