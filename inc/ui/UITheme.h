#pragma once
#include "UIStyle.h"
#include <memory>
#include <unordered_map>
#include <string>

namespace ui {

    class UITheme {
    public:
        UITheme();
        ~UITheme() = default;

        // Retrieve the style for a given component type and an optional element ID.
        // If elementId is empty or not found, returns the "default" style for that component.
        std::shared_ptr<UIStyle> getStyle(const std::string& componentType, const std::string& elementId = "") const;

        // Set the style for a given component type and element ID.
        void setStyle(const std::string& componentType, const std::string& elementId, std::shared_ptr<UIStyle> style);

        // Load a theme from a JSON file.
        bool loadFromJSON(const std::string& filePath);

        void clearStyles();

    private:
        // Map: componentType -> (elementId -> style)
        std::unordered_map<std::string, std::unordered_map<std::string, std::shared_ptr<UIStyle>>> styles_;
    };

} // namespace ui
