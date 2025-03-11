#include "UIFactory.h"
#include "UIButton.h"
#include "UITextField.h"
#include "UICheckBox.h"
#include "UIImage.h"
#include "UILabel.h"
#include "UISlider.h"
#include "UIRadioButton.h"
#include "UIManager.h"
#include <spdlog/spdlog.h>

namespace ui {

std::unique_ptr<UICanvas> UIFactory::createCanvas(const std::string& styleType, int zIndex) {
    auto canvas = std::make_unique<UICanvas>(styleType, zIndex);
    UIManager::getInstance().addCanvas(canvas.get());
    return canvas;
}

std::unique_ptr<UIDockable> UIFactory::createDockable(const std::string& title, const std::string& styleType, int zIndex) {
    auto dockable = UIDockable::create(title, styleType, zIndex);
    UIManager::getInstance().addCanvas(dockable.get());
    return dockable;
}

std::unique_ptr<UIDialog> UIFactory::createDialog(
    const std::string& title,
    const std::string& message,
    UIDialog::DialogType dialogType,
    UIDialog::ButtonType buttonType,
    const std::string& styleType,
    int zIndex
) {
    auto dialog = UIDialog::create(title, message, dialogType, buttonType, styleType, zIndex);
    UIManager::getInstance().addCanvas(dialog.get());
    return dialog;
}

std::unique_ptr<UINormalWindow> UIFactory::createNormalWindow(const std::string& title, const std::string& styleType, int zIndex) {
    auto window = UINormalWindow::create(title, styleType, zIndex);
    UIManager::getInstance().addCanvas(window.get());
    return window;
}

std::unique_ptr<UIToolbar> UIFactory::createToolbar(UIBoxLayout::Orientation orientation, const std::string& styleType, int zIndex) {
    auto toolbar = UIToolbar::create(orientation, styleType, zIndex);
    UIManager::getInstance().addCanvas(toolbar.get());
    return toolbar;
}

std::unique_ptr<UIElement> UIFactory::createElement(const std::string& type, UICanvas* parent) {
    std::unique_ptr<UIElement> element;
    if (type == "label") {
        auto label = UILabel::create();
        label->setTextAlignment(UIElement::TextAlignment::Center);
        element = std::move(label);
    } else if (type == "button") {
        auto button = UIButton::create();
        button->setTextAlignment(UIElement::TextAlignment::Center);
        element = std::move(button);
    } else if (type == "textField") {
        auto textField = UITextField::create();
        textField->setTextAlignment(UIElement::TextAlignment::Left);
        element = std::move(textField);
    } else if (type == "checkBox") {
        auto checkBox = UICheckBox::create();
        checkBox->setTextAlignment(UIElement::TextAlignment::Right);
        element = std::move(checkBox);
    } else if (type == "radioButton") {
        auto radioButton = UIRadioButton::create();
        radioButton->setTextAlignment(UIElement::TextAlignment::Right);
        element = std::move(radioButton);
    } else if (type == "slider") {
        auto slider = UISlider::create();
        slider->setTextAlignment(UIElement::TextAlignment::CenterTop);
        element = std::move(slider);
    } else if (type == "image") {
        element = UIImage::create();
    } else if (type == "dockable") {
        auto dockable = UIDockable::create("Dockable");
        element = std::move(dockable);
    } else if (type == "propertyPane") {
        auto propertyPane = UIPropertyPane::create("Property Pane");
        element = std::move(propertyPane);
    } else {
        spdlog::warn("Unknown element type: {}", type);
        return nullptr;
    }

    if (parent && element) {
        parent->addChild(element.get());
    }
    return element;
}

std::unique_ptr<UIElement> UIFactory::createElement(const std::string& type, UIPropertyPane* parent) {
    std::unique_ptr<UIElement> element = createElement(type, static_cast<UICanvas*>(parent));
    if (parent && element) {
        // Property-specific initialization (e.g., alignment within pane)
        if (type == "label" || type == "textField") {
            element->setTextAlignment(UIElement::TextAlignment::Left); // Default for property fields
        }
        parent->addChild(element.get());
    }
    return element;
}

} // namespace ui