#pragma once
#include <string>
#include <glm/glm.hpp>

namespace ui {

class IIcon {
public:
    virtual ~IIcon() = default;

    virtual std::string getName() const = 0;
    virtual glm::vec2 getSize() const = 0; // Width and height of the icon
    // Additional methods (e.g., getTexture()) can be added later
};

} // namespace ui