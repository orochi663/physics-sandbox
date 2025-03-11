#pragma once
#include <glm/glm.hpp>
#include <memory>
#include <unordered_map>

namespace ui {

class UITexture;

class UIStyle {
public:
    UIStyle();
    ~UIStyle() = default;

    // Base properties
    glm::vec4 backgroundColor{0.5f, 0.5f, 0.5f, 1.0f};
    std::shared_ptr<UITexture> backgroundTexture;
    glm::vec4 textColor{1.0f, 1.0f, 1.0f, 1.0f};
    float fontSize{16.0f};
    std::string fontName{"default"};
    glm::vec4 separatorColor{0.5f, 0.5f, 0.5f, 1.0f}; // Default gray separator color
    bool useBeautification{false}; // Flag to enable/disable separators

    // State-based overrides
    struct StateStyle {
        glm::vec4 backgroundColor{0.0f, 0.0f, 0.0f, 0.0f}; // Default to transparent
        std::shared_ptr<UITexture> backgroundTexture;
        glm::vec4 separatorColor{0.0f, 0.0f, 0.0f, 0.0f}; // Default to transparent
        bool useBeautification{false}; // Default to false
    };
    std::unordered_map<std::string, StateStyle> stateStyles; // e.g., "hovered", "pressed"

    // Merge functionality
    void merge(const UIStyle& other);
    UIStyle computeEffectiveStyle(const std::unordered_map<std::string, bool>& states) const;
};

} // namespace ui