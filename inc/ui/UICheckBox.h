#pragma once
#include "UIElement.h"
#include <functional>

namespace ui {

class UILabel;

class UICheckBox : public UIElement {
public:
    static std::unique_ptr<UICheckBox> create(const std::string& labelText = "Check Me");
    void render(IRenderer* renderer) override;
    bool handleInput(IMouseEvent* mouseEvent) override;
    bool handleInput(IKeyboardEvent* keyboardEvent) override;

    void setChecked(bool checked);
    bool isChecked() const { return checked_; }
    void setOnChecked(std::function<void(bool)> callback) { onChecked_ = callback; }

private:
    UICheckBox(const std::string& labelText);
    bool checked_{false};
    std::unique_ptr<UILabel> label_;
    std::function<void(bool)> onChecked_;
};

} // namespace ui