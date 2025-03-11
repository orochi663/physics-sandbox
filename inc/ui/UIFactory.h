#pragma once
#include "UIElement.h"
#include "UICanvas.h"
#include "UIDockable.h"
#include "UIDialog.h"
#include "UINormalWindow.h"
#include "UIToolbar.h"
#include "UIPropertyPane.h"
#include <memory>

namespace ui {

class UIFactory {
public:
    static std::unique_ptr<UICanvas> createCanvas(const std::string& styleType = "canvas", int zIndex = 0);
    static std::unique_ptr<UIDockable> createDockable(const std::string& title, const std::string& styleType = "dockable", int zIndex = 0);
    static std::unique_ptr<UIDialog> createDialog(
        const std::string& title,
        const std::string& message,
        UIDialog::DialogType dialogType,
        UIDialog::ButtonType buttonType,
        const std::string& styleType = "dialog",
        int zIndex = 1000
    );
    static std::unique_ptr<UINormalWindow> createNormalWindow(const std::string& title, const std::string& styleType = "window", int zIndex = 0);
    static std::unique_ptr<UIToolbar> createToolbar(UIBoxLayout::Orientation orientation = UIBoxLayout::Orientation::Horizontal, const std::string& styleType = "toolbar", int zIndex = 0);
    static std::unique_ptr<UIElement> createElement(const std::string& type, UICanvas* parent = nullptr);
    static std::unique_ptr<UIElement> createElement(const std::string& type, UIPropertyPane* parent = nullptr);

private:
    UIFactory() = default;
    ~UIFactory() = default;
};

} // namespace ui