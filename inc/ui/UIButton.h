#pragma once
#include "UIElement.h"
#include <memory>

namespace ui {

class UILabel;

class UIButton : public UIElement {
public:
    static std::unique_ptr<UIButton> create(const std::string& labelText = "Click Me");
    void render(IRenderer* renderer) override;
    bool handleInput(IMouseEvent* mouseEvent) override;
    bool handleInput(IKeyboardEvent* keyboardEvent) override;
    bool handleInput(ITextInputEvent* textEvent) override;

    void setText(const std::string& text) { label_->setText(text); markDirty(); }
    void setOnClick(std::function<void()> callback) { onClick_ = callback; }

private:
    UIButton(const std::string& labelText);
    std::unique_ptr<UILabel> label_;
    std::function<void()> onClick_;
};

} // namespace ui