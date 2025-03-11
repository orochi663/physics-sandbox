#include "UITextField.h"
#include "UICanvas.h"
#include <spdlog/spdlog.h>

namespace ui {

std::unique_ptr<UITextField> UITextField::create() {
    return std::unique_ptr<UITextField>(new UITextField());
}

UITextField::UITextField() {
    styleType_ = "textField";
    size_ = glm::vec2(150.0f, 20.0f);
    setTextAlignment(TextAlignment::Left); // Default to Left for text entry
    registerEventHandler("styleUpdate", [this](UIElement*, EventType) { onStyleUpdate(); });
}

void UITextField::setText(const std::string& text) {
    text_ = text;
    cursorPos_ = text_.length();
    markDirty();
}

void UITextField::render(IRenderer* renderer) {
    if (!renderer || !dirty_) return;

    const UITheme* theme = nullptr;
    if (auto* canvas = dynamic_cast<UICanvas*>(parent_.value_or(nullptr))) {
        theme = canvas->getEffectiveTheme();
    }
    if (!theme) return;

    UIStyle style = theme->getStyle(styleType_);
    std::unordered_map<std::string, bool> states = {{"hovered", isHovered_}, {"pressed", isPressed_}, {"focused", hasFocus()}};
    UIStyle effectiveStyle = style.computeEffectiveStyle(states);

    renderer->drawRect(position_, size_, effectiveStyle.backgroundColor);

    // Render text based on alignment
    glm::vec2 textPos = position_;
    switch (getTextAlignment()) {
        case TextAlignment::Left:
            textPos.x += 5.0f;
            break;
        case TextAlignment::Right:
            textPos.x += size_.x - renderer->measureText(text_, style.fontSize).x - 5.0f;
            break;
        case TextAlignment::Center:
            textPos.x += (size_.x - renderer->measureText(text_, style.fontSize).x) / 2.0f;
            break;
        default:
            textPos.x += 5.0f; // Default to Left
            break;
    }
    textPos.y += (size_.y + style.fontSize) / 2.0f;
    renderer->drawText(textPos, text_, effectiveStyle.textColor, style.fontSize);

    // Render cursor if focused
    if (hasFocus()) {
        float cursorX = textPos.x + renderer->measureText(text_.substr(0, cursorPos_), style.fontSize).x;
        renderer->drawLine(glm::vec2(cursorX, textPos.y - style.fontSize), glm::vec2(cursorX, textPos.y), effectiveStyle.textColor);
    }

    dirty_ = false;
}

bool UITextField::handleInput(IMouseEvent* mouseEvent) {
    if (!mouseEvent) return false;

    bool handled = UIElement::handleInput(mouseEvent);
    if (handled && mouseEvent->getType() == EventType::MousePress && hitTest(mouseEvent->getPosition())) {
        setFocus(true);
        return true;
    }
    return handled;
}

bool UITextField::handleInput(IKeyboardEvent* keyboardEvent) {
    if (!keyboardEvent || !hasFocus()) return false;

    if (keyboardEvent->getType() == EventType::KeyPress) {
        if (keyboardEvent->getKeyCode() == KeyCode::Left) {
            if (cursorPos_ > 0) --cursorPos_;
            markDirty();
            return true;
        } else if (keyboardEvent->getKeyCode() == KeyCode::Right) {
            if (cursorPos_ < static_cast<int>(text_.length())) ++cursorPos_;
            markDirty();
            return true;
        } else if (keyboardEvent->getKeyCode() == KeyCode::Backspace) {
            if (cursorPos_ > 0) {
                text_.erase(cursorPos_ - 1, 1);
                --cursorPos_;
                markDirty();
            }
            return true;
        }
    }
    return UIElement::handleInput(keyboardEvent);
}

bool UITextField::handleInput(ITextInputEvent* textEvent) {
    if (!textEvent || !hasFocus()) return false;

    text_ = text_.substr(0, cursorPos_) + textEvent->getText() + text_.substr(cursorPos_);
    cursorPos_ += textEvent->getText().length();
    markDirty();
    return true;
}

} // namespace ui