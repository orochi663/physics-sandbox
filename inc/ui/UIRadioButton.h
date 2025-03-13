#pragma once
#include "ui/UIElement.h"
#include <functional>
#include <memory>
#include <string>

namespace ui {

    class UILabel;

    class UIRadioButton : public UIElement {
    public:
        static std::unique_ptr<UIRadioButton> create(const std::string& labelText = "Option");
        void render(IRenderer* renderer) override;
        bool handleInput(IMouseEvent* mouseEvent) override;
        bool handleInput(IKeyboardEvent* keyboardEvent) override;

        void setChecked(bool checked);
        bool isChecked() const { return checked_; }
        void setOnChecked(std::function<void(bool)> callback) { onChecked_ = std::move(callback); }

        void onStyleUpdate() override;

    private:
        UIRadioButton(const std::string& labelText);
        bool checked_{ false };
        std::unique_ptr<UILabel> label_;
        std::function<void(bool)> onChecked_;
    };

} // namespace ui
