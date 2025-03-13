#pragma once
#include "ui/IRenderer.h"
#include <glm/glm.hpp>
#include <string>

namespace ui {

class UITooltip {
public:
    UITooltip(const std::string& text);
    ~UITooltip() = default;

    void setText(const std::string& text);
    std::string getText() const;

    void setDelay(float delay);
    float getDelay() const;

    // New: Render the tooltip using style settings from the "tooltip" style.
    // parentPos and parentSize indicate the bounds of the UIElement this tooltip belongs to.
    void render(IRenderer* renderer, const glm::vec2& parentPos, const glm::vec2& parentSize);

private:
    std::string text_;
    float delay_; // Delay before showing tooltip
};

} // namespace ui
