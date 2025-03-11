#include "UIRadioButtonGroup.h"
#include <spdlog/spdlog.h>

namespace ui {

UIRadioButtonGroup::UIRadioButtonGroup() {}

UIRadioButtonGroup::~UIRadioButtonGroup() {
    for (auto* radioButton : radioButtons_) {
        if (radioButton) {
            UIEventBus::getInstance().unregisterHandler("radioButtonChecked", radioButton);
        }
    }
}

void UIRadioButtonGroup::addRadioButton(UIRadioButton* radioButton) {
    if (!radioButton) return;

    radioButtons_.push_back(radioButton);
    UIEventBus::getInstance().registerHandler("radioButtonChecked", radioButton,
        [this](UIElement* sender, EventType eventType) { handleRadioButtonChecked(sender, eventType); });

    if (radioButton->isChecked() && selected_ != radioButton) {
        if (selected_) selected_->setChecked(false);
        selected_ = radioButton;
    }
}

void UIRadioButtonGroup::removeRadioButton(UIRadioButton* radioButton) {
    radioButtons_.erase(
        std::remove(radioButtons_.begin(), radioButtons_.end(), radioButton),
        radioButtons_.end()
    );
    UIEventBus::getInstance().unregisterHandler("radioButtonChecked", radioButton);
    if (selected_ == radioButton) {
        selected_ = nullptr;
    }
}

void UIRadioButtonGroup::handleRadioButtonChecked(UIElement* sender, EventType eventType) {
    UIRadioButton* radioButton = dynamic_cast<UIRadioButton*>(sender);
    if (!radioButton || !radioButton->isChecked()) return;

    if (selected_ != radioButton) {
        if (selected_) selected_->setChecked(false);
        selected_ = radioButton;
        if (onSelectionChanged_) onSelectionChanged_(selected_);
    }
}

} // namespace ui