#include "ui/UITheme.h"
#include <fstream>
#include <sstream>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

namespace ui {

    UITheme::UITheme() {
        // Initialize default styles for each component type.
        styles_["button"]["default"] = std::make_shared<UIButtonStyle>();
        styles_["tooltip"]["default"] = std::make_shared<UITooltipStyle>();
        styles_["dialog"]["default"] = std::make_shared<UIDialogStyle>();
        styles_["checkBox"]["default"] = std::make_shared<UICheckBoxStyle>();
        styles_["radioButton"]["default"] = std::make_shared<UIRadioButtonStyle>();
        styles_["propertyPane"]["default"] = std::make_shared<UIPropertyPaneStyle>();
    }

    std::shared_ptr<UIStyle> UITheme::getStyle(const std::string& componentType, const std::string& elementId) const {
        auto compIt = styles_.find(componentType);
        if (compIt != styles_.end()) {
            // If an elementId is specified, try to find it.
            if (!elementId.empty()) {
                auto idIt = compIt->second.find(elementId);
                if (idIt != compIt->second.end()) {
                    return idIt->second;
                }
            }
            // Fall back to the "default" style.
            auto defaultIt = compIt->second.find("default");
            if (defaultIt != compIt->second.end()) {
                return defaultIt->second;
            }
        }
        // If not found, return a new default UIStyle.
        return std::make_shared<UIStyle>();
    }

    void UITheme::setStyle(const std::string& componentType, const std::string& elementId, std::shared_ptr<UIStyle> style) {
        std::string id = elementId.empty() ? "default" : elementId;
        styles_[componentType][id] = style;
    }

    bool UITheme::loadFromJSON(const std::string& filePath) {
        std::ifstream file(filePath);
        if (!file.is_open()) {
            spdlog::error("Failed to open theme file: {}", filePath);
            return false;
        }
        std::stringstream buffer;
        buffer << file.rdbuf();
        file.close();

        try {
            nlohmann::json j = nlohmann::json::parse(buffer.str());
            for (auto& [componentType, value] : j.items()) {
                // The JSON value can be an array (multiple styles) or an object (a single style).
                if (value.is_array()) {
                    for (auto& styleJson : value) {
                        std::string jsonStr = styleJson.dump();
                        std::string id = "default";
                        if (styleJson.contains("elementId"))
                            id = styleJson["elementId"].get<std::string>();

                        if (componentType == "button") {
                            auto btnStyle = std::make_shared<UIButtonStyle>();
                            btnStyle->loadFromJSON(jsonStr);
                            styles_[componentType][id] = btnStyle;
                        }
                        else if (componentType == "tooltip") {
                            auto tooltipStyle = std::make_shared<UITooltipStyle>();
                            tooltipStyle->loadFromJSON(jsonStr);
                            styles_[componentType][id] = tooltipStyle;
                        }
                        else if (componentType == "dialog") {
                            auto dialogStyle = std::make_shared<UIDialogStyle>();
                            dialogStyle->loadFromJSON(jsonStr);
                            styles_[componentType][id] = dialogStyle;
                        }
                        else if (componentType == "checkBox") {
                            auto checkBoxStyle = std::make_shared<UICheckBoxStyle>();
                            checkBoxStyle->loadFromJSON(jsonStr);
                            styles_[componentType][id] = checkBoxStyle;
                        }
                        else if (componentType == "radioButton") {
                            auto radioStyle = std::make_shared<UIRadioButtonStyle>();
                            radioStyle->loadFromJSON(jsonStr);
                            styles_[componentType][id] = radioStyle;
                        }
                        else if (componentType == "propertyPane") {
                            auto ppStyle = std::make_shared<UIPropertyPaneStyle>();
                            ppStyle->loadFromJSON(jsonStr);
                            styles_[componentType][id] = ppStyle;
                        }
                        else {
                            auto baseStyle = std::make_shared<UIStyle>();
                            baseStyle->loadFromJSON(jsonStr);
                            styles_[componentType][id] = baseStyle;
                        }
                    }
                }
                else if (value.is_object()) {
                    std::string jsonStr = value.dump();
                    std::string id = "default";
                    if (value.contains("elementId"))
                        id = value["elementId"].get<std::string>();
                    if (componentType == "button") {
                        auto btnStyle = std::make_shared<UIButtonStyle>();
                        btnStyle->loadFromJSON(jsonStr);
                        styles_[componentType][id] = btnStyle;
                    }
                    else if (componentType == "tooltip") {
                        auto tooltipStyle = std::make_shared<UITooltipStyle>();
                        tooltipStyle->loadFromJSON(jsonStr);
                        styles_[componentType][id] = tooltipStyle;
                    }
                    else if (componentType == "dialog") {
                        auto dialogStyle = std::make_shared<UIDialogStyle>();
                        dialogStyle->loadFromJSON(jsonStr);
                        styles_[componentType][id] = dialogStyle;
                    }
                    else if (componentType == "checkBox") {
                        auto checkBoxStyle = std::make_shared<UICheckBoxStyle>();
                        checkBoxStyle->loadFromJSON(jsonStr);
                        styles_[componentType][id] = checkBoxStyle;
                    }
                    else if (componentType == "radioButton") {
                        auto radioStyle = std::make_shared<UIRadioButtonStyle>();
                        radioStyle->loadFromJSON(jsonStr);
                        styles_[componentType][id] = radioStyle;
                    }
                    else if (componentType == "propertyPane") {
                        auto ppStyle = std::make_shared<UIPropertyPaneStyle>();
                        ppStyle->loadFromJSON(jsonStr);
                        styles_[componentType][id] = ppStyle;
                    }
                    else {
                        auto baseStyle = std::make_shared<UIStyle>();
                        baseStyle->loadFromJSON(jsonStr);
                        styles_[componentType][id] = baseStyle;
                    }
                }
            }
        }
        catch (std::exception& ex) {
            spdlog::error("Failed to parse theme JSON: {}", ex.what());
            return false;
        }
        return true;
    }

    void UITheme::clearStyles() {
        styles_.clear();
    }

} // namespace ui
