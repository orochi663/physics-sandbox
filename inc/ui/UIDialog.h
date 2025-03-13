#pragma once

#include "ui/UICanvas.h"
#include "ui/UILabel.h"
#include "ui/UIButton.h"
#include "ui/UIEventBus.h"
#include <memory>
#include <string>
#include <functional>

namespace ui {

    // Dialog types available for the dialog.
    class UIDialog : public UICanvas {
    public:
        enum class DialogType {
            Ok,
            OkCancel,
            YesNo
        };
        // ButtonType definition for dialog buttons.
        enum class ButtonType {
            Ok,
            Cancel,
            Yes,
            No
        };

        // Static factory method to create a UIDialog instance.
        static std::unique_ptr<UIDialog> create(const std::string& title,
            const std::string& message,
            DialogType type,
            ButtonType buttonType,
            const std::string& styleType,
            int zIndex = 100);

    protected:
        // Constructor now includes styleType.
        UIDialog(const std::string& title,
            const std::string& message,
            DialogType type,
            ButtonType buttonType,
            const std::string& styleType,
            int zIndex = 100);

    public:
        ~UIDialog() override;

        void render(IRenderer* renderer) override;
        void doRender(IRenderer* renderer); // Called by the render thread

        void setOnClose(std::function<void()> callback) { onClose_ = std::move(callback); }
        void close();

        virtual void updateLayout() override;
        void onStyleUpdate() override;

    private:
        struct FadeTask; // For fade-in/fade-out animation.
        FadeTask fadeIn();

        void configureButtons();
        void registerEventHandlers();
        void onKeyPress(UIElement* element, EventType event);

        DialogType type_;
        ButtonType buttonType_;
        float opacity_; // For fade-in effect.
        std::unique_ptr<UILabel> titleLabel_;
        std::unique_ptr<UILabel> messageLabel_;
        std::function<void()> onClose_;
    };

} // namespace ui

