#pragma once
#include "UICanvas.h"

namespace ui {

class UIDialog : public UICanvas {
public:
    enum class DialogType { Alert, Warning, Info, Confirm };
    enum class ButtonType { OK, OK_CANCEL, YES_NO, YES_NO_CANCEL, CANCEL };
    enum class DialogResult { None, OK, Cancel, Yes, No };

    static std::unique_ptr<UIDialog> create(
        const std::string& title,
        const std::string& message,
        DialogType dialogType,
        ButtonType buttonType,
        const std::string& styleType = "dialog",
        int zIndex = 1000
    );
    void render(IRenderer* renderer) override;
    bool handleInput(IKeyboardEvent* keyboardEvent) override;

    DialogResult getResult() const { return result_; }
    void setOnResult(std::function<void(DialogResult)> callback) { onResult_ = callback; }

private:
    UIDialog(const std::string& title, const std::string& message, DialogType dialogType, ButtonType buttonType, const std::string& styleType, int zIndex);
    void setupButtons(ButtonType buttonType);
    void close(DialogResult result);

    std::string title_;
    std::string message_;
    DialogType dialogType_;
    DialogResult result_{DialogResult::None};
    std::unique_ptr<UILabel> titleLabel_;
    std::unique_ptr<UILabel> messageLabel_;
    std::function<void(DialogResult)> onResult_;
    float titleBarHeight_{20.0f};
};

} // namespace ui