#pragma once
#include "ui/UIElement.h"
#include <memory>
#include <functional>
#include <string>

namespace ui {

    class UILabel;
    class ITexture;

    class UIButton : public UIElement {
    public:
        static std::unique_ptr<UIButton> create(const std::string& labelText = "Click Me");
        void render(IRenderer* renderer) override;
        bool handleInput(IMouseEvent* mouseEvent) override;
        bool handleInput(IKeyboardEvent* keyboardEvent) override;

        void setText(const std::string& text);
        std::string getText() const;

        void setOnClick(std::function<void()> callback) { onClick_ = std::move(callback); }

        // New functions to support an icon/texture on the button.
        void setIcon(const std::shared_ptr<ITexture>& icon);
        std::shared_ptr<ITexture> getIcon() const { return icon_; }

        // Provide concrete implementation for onStyleUpdate.
        void onStyleUpdate() override;

    private:
        UIButton(const std::string& labelText);

        // The button may display text via a label.
        std::unique_ptr<UILabel> label_;
        // Optional icon to render on the button.
        std::shared_ptr<ITexture> icon_;
        std::function<void()> onClick_;
    };

} // namespace ui
