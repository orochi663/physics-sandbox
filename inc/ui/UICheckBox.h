#pragma once
#include "ui/UIElement.h"
#include <functional>
#include <memory>
#include <string>

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
        void setOnChecked(std::function<void(bool)> callback) { onChecked_ = std::move(callback); }

        void onStyleUpdate() override;

    private:
        UICheckBox(const std::string& labelText);
        bool checked_{ false };
        std::unique_ptr<UILabel> label_;
        std::function<void(bool)> onChecked_;
    };

} // namespace ui
