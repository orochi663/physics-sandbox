#pragma once
#include "UIElement.h"
#include <string>

namespace ui {

class UITextField : public UIElement {
public:
    static std::unique_ptr<UITextField> create();
    void render(IRenderer* renderer) override;
    bool handleInput(IMouseEvent* mouseEvent) override;
    bool handleInput(IKeyboardEvent* keyboardEvent) override;
    bool handleInput(ITextInputEvent* textEvent) override;

    void setText(const std::string& text);
    std::string getText() const { return text_; }

private:
    UITextField();
    std::string text_;
    int cursorPos_{0};
};

} // namespace ui