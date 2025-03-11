#include "UIDialog.h"
#include "UILabel.h"
#include "UIButton.h"
#include "CanvasFactory.h"
#include <spdlog/spdlog.h>

namespace ui {

std::unique_ptr<UIDialog> UIDialog::create(
    const std::string& title,
    const std::string& message,
    DialogType dialogType,
    ButtonType buttonType,
    const std::string& styleType,
    int zIndex
) {
    return std::unique_ptr<UIDialog>(new UIDialog(title, message, dialogType, buttonType, styleType, zIndex));
}

UIDialog::UIDialog(const std::string& title, const std::string& message, DialogType dialogType, ButtonType buttonType, const std::string& styleType, int zIndex)
    : UICanvas(styleType, zIndex), title_(title), message_(message), dialogType_(dialogType) {
    isModal_ = true;
    size_ = glm::vec2(300.0f, 150.0f);
    position_ = glm::vec2(1280.0f / 2.0f - size_.x / 2.0f, 720.0f / 2.0f - size_.y / 2.0f); // Center on screen

    titleLabel_ = std::make_unique<UILabel>(this);
    titleLabel_->setText(title_);
    titleLabel_->setStyleType("dialogTitle");
    setTextAlignment(TextAlignment::Left); // Default to Left as per original design

    messageLabel_ = std::make_unique<UILabel>(this);
    messageLabel_->setText(message_);
    messageLabel_->setStyleType("dialogMessage");
    messageLabel_->setTextAlignment(TextAlignment::Left); // Default to Left

    setupButtons(buttonType);
    registerEventHandler("styleUpdate", [this](UIElement*, EventType) { onStyleUpdate(); });
}

void UIDialog::setupButtons(ButtonType buttonType) {
    float buttonY = position_.y + size_.y - 40.0f;
    float buttonSpacing = 10.0f;
    std::vector<std::pair<std::string, DialogResult>> buttons;

    switch (buttonType) {
        case ButtonType::OK:
            buttons = {{"OK", DialogResult::OK}};
            break;
        case ButtonType::OK_CANCEL:
            buttons = {{"OK", DialogResult::OK}, {"Cancel", DialogResult::Cancel}};
            break;
        case ButtonType::YES_NO:
            buttons = {{"Yes", DialogResult::Yes}, {"No", DialogResult::No}};
            break;
        case ButtonType::YES_NO_CANCEL:
            buttons = {{"Yes", DialogResult::Yes}, {"No", DialogResult::No}, {"Cancel", DialogResult::Cancel}};
            break;
        case ButtonType::CANCEL:
            buttons = {{"Cancel", DialogResult::Cancel}};
            break;
    }

    float totalWidth = buttons.size() * 80.0f + (buttons.size() - 1) * buttonSpacing;
    float startX = position_.x + (size_.x - totalWidth) / 2.0f;

    for (size_t i = 0; i < buttons.size(); ++i) {
        auto& [text, result] = buttons[i];
        auto button = CanvasFactory::createElement("button", this);
        auto* btn = dynamic_cast<UIButton*>(button.get());
        if (btn) {
            btn->setPosition(glm::vec2(startX + i * (80.0f + buttonSpacing), buttonY));
            btn->setSize(glm::vec2(80.0f, 30.0f));
            btn->setText(text);
            btn->setOnClick([this, result]() { close(result); });
            addChild(std::move(button));
        }
    }
}

void UIDialog::close(DialogResult result) {
    result_ = result;
    if (onResult_) onResult_(result_);
    UIManager::getInstance().removeCanvas(this);
}

void UIDialog::render(IRenderer* renderer) {
    if (!renderer || !isVisible_ || !dirty_) return;

    const UITheme* theme = getEffectiveTheme();
    if (!theme) return;

    UIStyle style = theme->getStyle(styleType_);
    std::unordered_map<std::string, bool> states = {{"hovered", isHovered_}, {"pressed", isPressed_}};
    UIStyle effectiveStyle = style.computeEffectiveStyle(states);

    // Render background
    renderer->drawRect(position_, size_, effectiveStyle.backgroundColor);

    // Render title bar
    glm::vec2 titleBarPos = position_;
    glm::vec2 titleBarSize(size_.x, titleBarHeight_);
    renderer->drawRect(titleBarPos, titleBarSize, effectiveStyle.backgroundColor);

    // Position title label based on text alignment
    glm::vec2 titleLabelPos = titleBarPos;
    switch (getTextAlignment()) {
        case TextAlignment::Left:
            titleLabelPos.x += 5.0f;
            break;
        case TextAlignment::Right:
            titleLabelPos.x += titleBarSize.x - titleLabel_->getSize().x - 5.0f;
            break;
        default:
            titleLabelPos.x += 5.0f; // Default to Left
            break;
    }
    titleLabel_->setPosition(titleLabelPos);
    titleLabel_->render(renderer);

    // Position message label based on its alignment
    glm::vec2 messageLabelPos = position_ + glm::vec2(10.0f, titleBarHeight_ + 10.0f);
    switch (messageLabel_->getTextAlignment()) {
        case TextAlignment::Left:
            messageLabelPos.x = position_.x + 10.0f;
            break;
        case TextAlignment::Right:
            messageLabelPos.x = position_.x + size_.x - messageLabel_->getSize().x - 10.0f;
            break;
        case TextAlignment::Center:
            messageLabelPos.x = position_.x + (size_.x - messageLabel_->getSize().x) / 2.0f;
            break;
        default:
            messageLabelPos.x = position_.x + 10.0f; // Default to Left
            break;
    }
    messageLabel_->setPosition(messageLabelPos);
    messageLabel_->render(renderer);

    // Render children (buttons)
    for (const auto& child : children_) {
        if (child) child->render(renderer);
    }

    dirty_ = false;
}

bool UIDialog::handleInput(IKeyboardEvent* keyboardEvent) {
    if (!keyboardEvent || !isVisible_) return false;

    if (keyboardEvent->getType() == EventType::KeyPress && keyboardEvent->getKeyCode() == KeyCode::Escape) {
        close(DialogResult::Cancel);
        return true;
    }

    return UICanvas::handleInput(keyboardEvent);
}

} // namespace ui