#include "ui/UIFactory.h"
#include "ui/UIButton.h"
#include "ui/UITextField.h"
#include "ui/UICheckBox.h"
#include "ui/UIImage.h"
#include "ui/UILabel.h"
#include "ui/UISlider.h"
#include "ui/UIRadioButton.h"
#include "ui/UIManager.h"
#include <spdlog/spdlog.h>

namespace ui {

    std::unique_ptr<UICanvas> UIFactory::createCanvas(const std::string& styleType, int zIndex) {
        return UICanvas::create(styleType, zIndex);
    }

    std::unique_ptr<UIDockable> UIFactory::createDockable(const std::string& title, const std::string& styleType, int zIndex) {
        return UIDockable::create(title, styleType, zIndex);
    }

    std::unique_ptr<UIDialog> UIFactory::createDialog(
        const std::string& title,
        const std::string& message,
        UIDialog::DialogType dialogType,
        UIDialog::ButtonType buttonType,
        const std::string& styleType,
        int zIndex)
    {
        return UIDialog::create(title, message, dialogType, buttonType, styleType, zIndex);
    }

    std::unique_ptr<UINormalWindow> UIFactory::createNormalWindow(const std::string& title, const std::string& styleType, int zIndex) {
        return UINormalWindow::create(title, styleType, zIndex);
    }

    std::unique_ptr<UIToolbar> UIFactory::createToolbar(UIBoxLayout::Orientation orientation, const std::string& styleType, int zIndex) {
        return UIToolbar::create(orientation, styleType, zIndex);
    }

    std::unique_ptr<UIElement> UIFactory::createElement(const std::string& type, UICanvas* parent) {
        std::unique_ptr<UIElement> element;
        if (type == "label") {
            element = UILabel::create();
            element->setTextAlignment(UIElement::TextAlignment::Center);
        }
        else if (type == "button") {
            element = UIButton::create("Button");
            element->setTextAlignment(UIElement::TextAlignment::Center);
        }
        else if (type == "textField") {
            element = UITextField::create();
            element->setTextAlignment(UIElement::TextAlignment::Left);
        }
        else if (type == "checkBox") {
            element = UICheckBox::create("Check Me");
            element->setTextAlignment(UIElement::TextAlignment::Right);
        }
        else if (type == "radioButton") {
            element = UIRadioButton::create("Option");
            element->setTextAlignment(UIElement::TextAlignment::Right);
        }
        else if (type == "slider") {
            element = UISlider::create();
            element->setTextAlignment(UIElement::TextAlignment::CenterTop);
        }
        else if (type == "image") {
            element = UIImage::create();
        }
        else if (type == "dockable") {
            element = UIDockable::create("Dockable");
        }
        else if (type == "propertyPane") {
            element = UIPropertyPane::create("Property Pane");
        }
        else {
            spdlog::warn("Unknown element type: {}", type);
            return nullptr;
        }
        if (parent && element) {
            parent->addChild(std::move(element));
        }
        return element;
    }

    std::unique_ptr<UIElement> UIFactory::createElement(const std::string& type, UIPropertyPane* parent) {
        std::unique_ptr<UIElement> element = createElement(type, static_cast<UICanvas*>(parent));
        if (parent && element) {
            element->setTextAlignment(UIElement::TextAlignment::Left);
            parent->addChild(std::move(element));
        }
        return element;
    }

} // namespace ui
