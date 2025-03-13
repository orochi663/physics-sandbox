#include "ui/UIStyle.h"
#include <sstream>
#include <nlohmann/json.hpp>

namespace ui {

    //////////////////////
    // UIStyle Base Class
    //////////////////////
    UIStyle::UIStyle()
        : elementId(""),
        backgroundColor(0.5f, 0.5f, 0.5f, 1.0f),
        textColor(1.0f, 1.0f, 1.0f, 1.0f),
        fontSize(16.0f),
        fontName("default"),
        padding(5.0f)
    {
    }

    void UIStyle::merge(const UIStyle& other) {
        if (other.backgroundColor != glm::vec4(0.0f))
            backgroundColor = other.backgroundColor;
        if (!other.fontName.empty())
            fontName = other.fontName;
        if (other.fontSize > 0.0f)
            fontSize = other.fontSize;
        if (other.textColor != glm::vec4(0.0f))
            textColor = other.textColor;
        if (other.padding > 0.0f)
            padding = other.padding;
        if (other.backgroundTexture)
            backgroundTexture = other.backgroundTexture;
        if (!other.elementId.empty())
            elementId = other.elementId;
    }

    void UIStyle::loadFromJSON(const std::string& jsonData) {
        try {
            nlohmann::json j = nlohmann::json::parse(jsonData);
            if (j.contains("elementId"))
                elementId = j["elementId"].get<std::string>();
            if (j.contains("backgroundColor") && j["backgroundColor"].is_array()) {
                auto arr = j["backgroundColor"];
                backgroundColor = glm::vec4(arr[0].get<float>(), arr[1].get<float>(), arr[2].get<float>(), arr[3].get<float>());
            }
            if (j.contains("textColor") && j["textColor"].is_array()) {
                auto arr = j["textColor"];
                textColor = glm::vec4(arr[0].get<float>(), arr[1].get<float>(), arr[2].get<float>(), arr[3].get<float>());
            }
            if (j.contains("fontSize"))
                fontSize = j["fontSize"].get<float>();
            if (j.contains("fontName"))
                fontName = j["fontName"].get<std::string>();
            if (j.contains("padding"))
                padding = j["padding"].get<float>();
            // Note: Loading backgroundTexture would require integration with a resource loader.
        }
        catch (std::exception& e) {
            // Error handling (log if needed)
        }
    }

    UIStyle UIStyle::computeEffectiveStyle(const std::unordered_map<std::string, bool>& states) const {
        // For simplicity, we return a copy of this style.
        // In a real implementation, you would modify the style based on the states.
        UIStyle effective = *this;
        return effective;
    }

    //////////////////////
    // UIButtonStyle
    //////////////////////
    UIButtonStyle::UIButtonStyle()
        : UIStyle(), borderRadius(4.0f), iconAlignment("center")
    {
    }

    void UIButtonStyle::merge(const UIStyle& other) {
        UIStyle::merge(other);
    }

    void UIButtonStyle::loadFromJSON(const std::string& jsonData) {
        nlohmann::json j = nlohmann::json::parse(jsonData);
        UIStyle::loadFromJSON(jsonData);
        if (j.contains("borderRadius"))
            borderRadius = j["borderRadius"].get<float>();
        if (j.contains("iconAlignment"))
            iconAlignment = j["iconAlignment"].get<std::string>();
    }

    //////////////////////
    // UITooltipStyle
    //////////////////////
    UITooltipStyle::UITooltipStyle()
        : UIStyle(), fadeInTime(0.3f), fadeOutTime(0.3f),
        shape(TooltipShape::Rectangle), pointerSize(10.0f), pointerOffset(5.0f)
    {
    }

    void UITooltipStyle::merge(const UIStyle& other) {
        UIStyle::merge(other);
    }

    void UITooltipStyle::loadFromJSON(const std::string& jsonData) {
        nlohmann::json j = nlohmann::json::parse(jsonData);
        UIStyle::loadFromJSON(jsonData);
        if (j.contains("fadeInTime"))
            fadeInTime = j["fadeInTime"].get<float>();
        if (j.contains("fadeOutTime"))
            fadeOutTime = j["fadeOutTime"].get<float>();
        if (j.contains("shape")) {
            std::string s = j["shape"].get<std::string>();
            shape = (s == "balloon") ? TooltipShape::Balloon : TooltipShape::Rectangle;
        }
        if (j.contains("pointerSize"))
            pointerSize = j["pointerSize"].get<float>();
        if (j.contains("pointerOffset"))
            pointerOffset = j["pointerOffset"].get<float>();
    }

    //////////////////////
    // UIDialogStyle
    //////////////////////
    UIDialogStyle::UIDialogStyle()
        : UIStyle(), fadeInTime(0.5f), fadeOutTime(0.5f), titleBarHeight(30.0f)
    {
    }

    void UIDialogStyle::merge(const UIStyle& other) {
        UIStyle::merge(other);
    }

    void UIDialogStyle::loadFromJSON(const std::string& jsonData) {
        nlohmann::json j = nlohmann::json::parse(jsonData);
        UIStyle::loadFromJSON(jsonData);
        if (j.contains("fadeInTime"))
            fadeInTime = j["fadeInTime"].get<float>();
        if (j.contains("fadeOutTime"))
            fadeOutTime = j["fadeOutTime"].get<float>();
        if (j.contains("titleBarHeight"))
            titleBarHeight = j["titleBarHeight"].get<float>();
    }

    //////////////////////
    // UICheckBoxStyle
    //////////////////////
    UICheckBoxStyle::UICheckBoxStyle()
        : UIStyle(), checkMarkColor(0.0f, 0.0f, 0.0f, 1.0f)
    {
    }

    void UICheckBoxStyle::merge(const UIStyle& other) {
        UIStyle::merge(other);
    }

    void UICheckBoxStyle::loadFromJSON(const std::string& jsonData) {
        nlohmann::json j = nlohmann::json::parse(jsonData);
        UIStyle::loadFromJSON(jsonData);
        if (j.contains("checkMarkColor") && j["checkMarkColor"].is_array()) {
            auto arr = j["checkMarkColor"];
            checkMarkColor = glm::vec4(arr[0].get<float>(), arr[1].get<float>(), arr[2].get<float>(), arr[3].get<float>());
        }
    }

    //////////////////////
    // UIRadioButtonStyle
    //////////////////////
    UIRadioButtonStyle::UIRadioButtonStyle()
        : UIStyle(), radioMarkColor(0.0f, 0.0f, 0.0f, 1.0f), radioMarkRadius(5.0f)
    {
    }

    void UIRadioButtonStyle::merge(const UIStyle& other) {
        UIStyle::merge(other);
    }

    void UIRadioButtonStyle::loadFromJSON(const std::string& jsonData) {
        nlohmann::json j = nlohmann::json::parse(jsonData);
        UIStyle::loadFromJSON(jsonData);
        if (j.contains("radioMarkColor") && j["radioMarkColor"].is_array()) {
            auto arr = j["radioMarkColor"];
            radioMarkColor = glm::vec4(arr[0].get<float>(), arr[1].get<float>(), arr[2].get<float>(), arr[3].get<float>());
        }
        if (j.contains("radioMarkRadius"))
            radioMarkRadius = j["radioMarkRadius"].get<float>();
    }

    //////////////////////
    // UINormalWindowStyle
    //////////////////////
    UINormalWindowStyle::UINormalWindowStyle()
        : UIStyle(), titleBarColor(0.2f, 0.2f, 0.2f, 1.0f),
        titleTextColor(1.0f, 1.0f, 1.0f, 1.0f),
        titleFontSize(16.0f),
        borderThickness(1.0f)
    {
    }

    void UINormalWindowStyle::merge(const UIStyle& other) {
        UIStyle::merge(other);
    }

    void UINormalWindowStyle::loadFromJSON(const std::string& jsonData) {
        nlohmann::json j = nlohmann::json::parse(jsonData);
        UIStyle::loadFromJSON(jsonData);
        if (j.contains("titleBarColor") && j["titleBarColor"].is_array()) {
            auto arr = j["titleBarColor"];
            titleBarColor = glm::vec4(arr[0].get<float>(), arr[1].get<float>(), arr[2].get<float>(), arr[3].get<float>());
        }
        if (j.contains("titleTextColor") && j["titleTextColor"].is_array()) {
            auto arr = j["titleTextColor"];
            titleTextColor = glm::vec4(arr[0].get<float>(), arr[1].get<float>(), arr[2].get<float>(), arr[3].get<float>());
        }
        if (j.contains("titleFontSize"))
            titleFontSize = j["titleFontSize"].get<float>();
        if (j.contains("borderThickness"))
            borderThickness = j["borderThickness"].get<float>();
    }

    //////////////////////
    // UIDockableStyle
    //////////////////////
    UIDockableStyle::UIDockableStyle()
        : UIStyle(), headerBackgroundColor(0.15f, 0.15f, 0.15f, 1.0f),
        headerTextColor(1.0f, 1.0f, 1.0f, 1.0f),
        headerFontSize(16.0f),
        borderThickness(1.0f)
    {
    }

    void UIDockableStyle::merge(const UIStyle& other) {
        UIStyle::merge(other);
    }

    void UIDockableStyle::loadFromJSON(const std::string& jsonData) {
        nlohmann::json j = nlohmann::json::parse(jsonData);
        UIStyle::loadFromJSON(jsonData);
        if (j.contains("headerBackgroundColor") && j["headerBackgroundColor"].is_array()) {
            auto arr = j["headerBackgroundColor"];
            headerBackgroundColor = glm::vec4(arr[0].get<float>(), arr[1].get<float>(), arr[2].get<float>(), arr[3].get<float>());
        }
        if (j.contains("headerTextColor") && j["headerTextColor"].is_array()) {
            auto arr = j["headerTextColor"];
            headerTextColor = glm::vec4(arr[0].get<float>(), arr[1].get<float>(), arr[2].get<float>(), arr[3].get<float>());
        }
        if (j.contains("headerFontSize"))
            headerFontSize = j["headerFontSize"].get<float>();
        if (j.contains("borderThickness"))
            borderThickness = j["borderThickness"].get<float>();
    }

    //////////////////////
    // UIScrollbarStyle
    //////////////////////
    UIScrollbarStyle::UIScrollbarStyle()
        : UIStyle(), thickness(20.0f),
        trackColor(0.8f, 0.8f, 0.8f, 1.0f),
        thumbColor(0.4f, 0.4f, 0.4f, 1.0f)
    {
    }

    void UIScrollbarStyle::merge(const UIStyle& other) {
        UIStyle::merge(other);
    }

    void UIScrollbarStyle::loadFromJSON(const std::string& jsonData) {
        nlohmann::json j = nlohmann::json::parse(jsonData);
        UIStyle::loadFromJSON(jsonData);
        if (j.contains("thickness"))
            thickness = j["thickness"].get<float>();
        if (j.contains("trackColor") && j["trackColor"].is_array()) {
            auto arr = j["trackColor"];
            trackColor = glm::vec4(arr[0].get<float>(), arr[1].get<float>(), arr[2].get<float>(), arr[3].get<float>());
        }
        if (j.contains("thumbColor") && j["thumbColor"].is_array()) {
            auto arr = j["thumbColor"];
            thumbColor = glm::vec4(arr[0].get<float>(), arr[1].get<float>(), arr[2].get<float>(), arr[3].get<float>());
        }
    }

    //////////////////////
    // UISliderStyle
    //////////////////////
    UISliderStyle::UISliderStyle()
        : UIStyle(), trackColor(0.8f, 0.8f, 0.8f, 1.0f),
        handleColor(0.3f, 0.3f, 0.3f, 1.0f)
    {
    }

    void UISliderStyle::merge(const UIStyle& other) {
        UIStyle::merge(other);
    }

    void UISliderStyle::loadFromJSON(const std::string& jsonData) {
        nlohmann::json j = nlohmann::json::parse(jsonData);
        UIStyle::loadFromJSON(jsonData);
        if (j.contains("trackColor") && j["trackColor"].is_array()) {
            auto arr = j["trackColor"];
            trackColor = glm::vec4(arr[0].get<float>(), arr[1].get<float>(), arr[2].get<float>(), arr[3].get<float>());
        }
        if (j.contains("handleColor") && j["handleColor"].is_array()) {
            auto arr = j["handleColor"];
            handleColor = glm::vec4(arr[0].get<float>(), arr[1].get<float>(), arr[2].get<float>(), arr[3].get<float>());
        }
    }

    //////////////////////
    // UILabelStyle
    //////////////////////
    UILabelStyle::UILabelStyle()
        : UIStyle(), lineSpacing(2.0f)
    {
    }

    void UILabelStyle::merge(const UIStyle& other) {
        UIStyle::merge(other);
    }

    void UILabelStyle::loadFromJSON(const std::string& jsonData) {
        nlohmann::json j = nlohmann::json::parse(jsonData);
        UIStyle::loadFromJSON(jsonData);
        if (j.contains("lineSpacing"))
            lineSpacing = j["lineSpacing"].get<float>();
    }

    //////////////////////
    // UIToolbarStyle
    //////////////////////
    UIToolbarStyle::UIToolbarStyle()
        : UIStyle(), spacing(5.0f)
    {
    }

    void UIToolbarStyle::merge(const UIStyle& other) {
        UIStyle::merge(other);
    }

    void UIToolbarStyle::loadFromJSON(const std::string& jsonData) {
        nlohmann::json j = nlohmann::json::parse(jsonData);
        UIStyle::loadFromJSON(jsonData);
        if (j.contains("spacing"))
            spacing = j["spacing"].get<float>();
    }

} // namespace ui
