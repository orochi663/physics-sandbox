#pragma once
#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <unordered_map>
#include <nlohmann/json.hpp>
#include "ITexture.h"

namespace ui {

    // For tooltip shape options.
    enum class TooltipShape {
        Rectangle,
        Balloon
    };

    // -------------------------------
    // Base UIStyle class: common properties for all UI elements.
    class UIStyle {
    public:
        UIStyle();
        virtual ~UIStyle() = default;

        // Unique identifier for the style (allows multiple variants).
        std::string elementId;

        // Common style properties.
        glm::vec4 backgroundColor;              // RGBA color.
        std::shared_ptr<ITexture> backgroundTexture; // Optional background texture.
        glm::vec4 textColor;                    // Text color.
        float fontSize;                         // Font size.
        std::string fontName;                   // Font name.
        float padding;                          // Uniform padding.

        // Merge the properties of another style into this one.
        virtual void merge(const UIStyle& other);
        // Load style properties from JSON data.
        virtual void loadFromJSON(const std::string& jsonData);
        // Compute effective style based on state flags (e.g., "hovered", "pressed").
        virtual UIStyle computeEffectiveStyle(const std::unordered_map<std::string, bool>& states) const;
    };

    // Specialized styles follow.

    class UIButtonStyle : public UIStyle {
    public:
        UIButtonStyle();
        virtual ~UIButtonStyle() = default;

        float borderRadius;                     // Button corner radius.
        std::string iconAlignment;              // "left", "right", or "center".

        void merge(const UIStyle& other) override;
        void loadFromJSON(const std::string& jsonData) override;
    };

    class UITooltipStyle : public UIStyle {
    public:
        UITooltipStyle();
        virtual ~UITooltipStyle() = default;

        float fadeInTime;    // Seconds to fade in.
        float fadeOutTime;   // Seconds to fade out.
        TooltipShape shape;  // Tooltip shape.
        float pointerSize;   // Size of pointer (if using balloon shape).
        float pointerOffset; // Offset for the pointer.

        void merge(const UIStyle& other) override;
        void loadFromJSON(const std::string& jsonData) override;
    };

    class UIDialogStyle : public UIStyle {
    public:
        UIDialogStyle();
        virtual ~UIDialogStyle() = default;

        float fadeInTime;    // Seconds to fade in dialog.
        float fadeOutTime;   // Seconds to fade out dialog.
        float titleBarHeight; // Height of the title bar.

        void merge(const UIStyle& other) override;
        void loadFromJSON(const std::string& jsonData) override;
    };

    class UICheckBoxStyle : public UIStyle {
    public:
        UICheckBoxStyle();
        virtual ~UICheckBoxStyle() = default;

        glm::vec4 checkMarkColor; // Color for the check mark.

        void merge(const UIStyle& other) override;
        void loadFromJSON(const std::string& jsonData) override;
    };

    class UIRadioButtonStyle : public UIStyle {
    public:
        UIRadioButtonStyle();
        virtual ~UIRadioButtonStyle() = default;

        glm::vec4 radioMarkColor; // Color for the radio mark.
        float radioMarkRadius;    // Radius for the radio mark.

        void merge(const UIStyle& other) override;
        void loadFromJSON(const std::string& jsonData) override;
    };

    class UINormalWindowStyle : public UIStyle {
    public:
        UINormalWindowStyle();
        virtual ~UINormalWindowStyle() = default;

        glm::vec4 titleBarColor;     // Background color of the title bar.
        glm::vec4 titleTextColor;    // Color of the title text.
        float titleFontSize;         // Font size for the title.
        float borderThickness;       // Border thickness around the window.

        void merge(const UIStyle& other) override;
        void loadFromJSON(const std::string& jsonData) override;
    };

    class UIDockableStyle : public UIStyle {
    public:
        UIDockableStyle();
        virtual ~UIDockableStyle() = default;

        glm::vec4 headerBackgroundColor; // Header background for dockable pane.
        glm::vec4 headerTextColor;       // Header text color.
        float headerFontSize;            // Font size for header.
        float borderThickness;           // Border thickness for dockable elements.

        void merge(const UIStyle& other) override;
        void loadFromJSON(const std::string& jsonData) override;
    };

    class UIScrollbarStyle : public UIStyle {
    public:
        UIScrollbarStyle();
        virtual ~UIScrollbarStyle() = default;

        float thickness;       // Scrollbar thickness.
        glm::vec4 trackColor;  // Color for the scrollbar track.
        glm::vec4 thumbColor;  // Color for the scrollbar thumb.

        void merge(const UIStyle& other) override;
        void loadFromJSON(const std::string& jsonData) override;
    };

    class UISliderStyle : public UIStyle {
    public:
        UISliderStyle();
        virtual ~UISliderStyle() = default;

        glm::vec4 trackColor;   // Color for the slider track.
        glm::vec4 handleColor;  // Color for the slider handle.

        void merge(const UIStyle& other) override;
        void loadFromJSON(const std::string& jsonData) override;
    };

    class UILabelStyle : public UIStyle {
    public:
        UILabelStyle();
        virtual ~UILabelStyle() = default;

        float lineSpacing;      // Additional spacing between lines.

        void merge(const UIStyle& other) override;
        void loadFromJSON(const std::string& jsonData) override;
    };

    class UIToolbarStyle : public UIStyle {
    public:
        UIToolbarStyle();
        virtual ~UIToolbarStyle() = default;

        float spacing;          // Spacing between items in the toolbar.

        void merge(const UIStyle& other) override;
        void loadFromJSON(const std::string& jsonData) override;
    };

} // namespace ui
