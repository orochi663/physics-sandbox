#pragma once
#include "UIRadioButton.h"
#include <vector>
#include <functional>

namespace ui {

class UIRadioButtonGroup {
public:
    UIRadioButtonGroup();
    ~UIRadioButtonGroup();

    void addRadioButton(UIRadioButton* radioButton);
    void removeRadioButton(UIRadioButton* radioButton);
    UIRadioButton* getSelected() const { return selected_; }
    void setOnSelectionChanged(std::function<void(UIRadioButton*)> callback) { onSelectionChanged_ = callback; }

private:
    void handleRadioButtonChecked(UIElement* sender, EventType eventType);

    std::vector<UIRadioButton*> radioButtons_;
    UIRadioButton* selected_{nullptr};
    std::function<void(UIRadioButton*)> onSelectionChanged_;
};

} // namespace ui