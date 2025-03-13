#include "ui/UIDialog.h"
#include "ui/UIButton.h"
#include "ui/UILabel.h"
#include "ui/UIFactory.h"
#include "ui/UIManager.h"
#include "ui/UIStyle.h"
#include <spdlog/spdlog.h>
#include <coroutine>
#include <chrono>

namespace ui {

    struct UIDialog::FadeTask {
        struct promise_type {
            FadeTask get_return_object() {
                return FadeTask{ std::coroutine_handle<promise_type>::from_promise(*this) };
            }
            std::suspend_never initial_suspend() noexcept { return {}; }
            std::suspend_always final_suspend() noexcept { return {}; }
            void return_void() noexcept {}
            void unhandled_exception() { spdlog::error("UIDialog FadeTask exception"); std::terminate(); }
        };
        std::coroutine_handle<promise_type> handle;
        explicit FadeTask(std::coroutine_handle<promise_type> h) : handle(h) {}
        ~FadeTask() { if (handle && !handle.done()) handle.destroy(); }
    };

    std::unique_ptr<UIDialog> UIDialog::create(const std::string& title,
        const std::string& message,
        DialogType type,
        ButtonType buttonType,
        const std::string& styleType,
        int zIndex)
    {
        return std::unique_ptr<UIDialog>(new UIDialog(title, message, type, buttonType, styleType, zIndex));
    }

    UIDialog::UIDialog(const std::string& title,
        const std::string& message,
        DialogType type,
        ButtonType buttonType,
        const std::string& styleType,
        int zIndex)
        : UICanvas(styleType, zIndex), type_(type), buttonType_(buttonType), opacity_(0.0f)
    {
        titleLabel_ = UILabel::create(title);
		titleLabel_->setParent(this);
        titleLabel_->setStyleType("dialogTitle");

        messageLabel_ = UILabel::create(message);
        titleLabel_->setParent(this);
        messageLabel_->setStyleType("dialogMessage");

        configureButtons();
        position_ = glm::vec2(0.0f); // Will be centered in updateLayout()
        size_ = glm::vec2(300.0f, 200.0f);
        updateLayout();
        fadeIn(); // Start fade-in animation
    }

    UIDialog::~UIDialog() {
        // Destructor logic if necessary.
    }

    UIDialog::FadeTask UIDialog::fadeIn() {
        float fadeInTime = 0.5f; // default fade-in time
        const UITheme* theme = getEffectiveTheme();
        if (theme) {
            auto stylePtr = std::dynamic_pointer_cast<UIDialogStyle>(theme->getStyle(styleType_));
            if (stylePtr) {
                fadeInTime = stylePtr->fadeInTime;
            }
        }
        int steps = 10;
        for (int i = 0; i <= steps; ++i) {
            opacity_ = static_cast<float>(i) / steps;
            markDirty();
            if (i < steps)
                co_await std::suspend_always{};
        }
        co_return;
    }

    void UIDialog::configureButtons() {
        auto& children = getMutableChildren();
        if (type_ == DialogType::Ok || type_ == DialogType::OkCancel) {
            auto okButton = UIFactory::createElement("button", this);
            UIButton* okBtn = dynamic_cast<UIButton*>(okButton.get());
            if (okBtn) {
                okBtn->setText("OK");
                okBtn->setOnClick([this]() { close(); });
            }
            children.push_back(std::move(okButton));
        }
        if (type_ == DialogType::OkCancel || type_ == DialogType::YesNo) {
            auto cancelButton = UIFactory::createElement("button", this);
            UIButton* cancelBtn = dynamic_cast<UIButton*>(cancelButton.get());
            if (cancelBtn) {
                cancelBtn->setText(type_ == DialogType::OkCancel ? "Cancel" : "No");
                cancelBtn->setOnClick([this]() { close(); });
            }
            children.push_back(std::move(cancelButton));
        }
        if (type_ == DialogType::YesNo) {
            auto& childrenVec = getMutableChildren();
            if (!childrenVec.empty() && childrenVec[0]) {
                if (auto btn = dynamic_cast<UIButton*>(childrenVec[0].get())) {
                    btn->setText("Yes");
                }
            }
        }
    }

    void UIDialog::updateLayout() {
        if (!parent_) {
            spdlog::warn("UIDialog has no parent canvas for layout computation");
            return;
        }
        glm::vec2 parentSize = parent_.value()->getSize();
        position_ = (parentSize - size_) / 2.0f; // Center dialog

        titleLabel_->setPosition(position_ + glm::vec2(10.0f, 10.0f));
        messageLabel_->setPosition(position_ + glm::vec2(10.0f, 40.0f));

        float buttonY = position_.y + size_.y - 40.0f;
        auto& children = getMutableChildren();
        if (type_ == DialogType::Ok) {
            if (!children.empty()) {
                children[0]->setPosition(glm::vec2(position_.x + size_.x / 2.0f - 30.0f, buttonY));
                children[0]->setSize(glm::vec2(60.0f, 30.0f));
            }
        }
        else if (type_ == DialogType::OkCancel || type_ == DialogType::YesNo) {
            if (children.size() >= 2) {
                children[0]->setPosition(glm::vec2(position_.x + size_.x / 2.0f - 70.0f, buttonY));
                children[0]->setSize(glm::vec2(60.0f, 30.0f));
                children[1]->setPosition(glm::vec2(position_.x + size_.x / 2.0f + 10.0f, buttonY));
                children[1]->setSize(glm::vec2(60.0f, 30.0f));
            }
        }
    }

    void UIDialog::render(IRenderer* renderer) {
        if (!renderer || !isVisible() || !isDirty()) return;
        UIManager::getInstance().queueForRender(this);
    }

    void UIDialog::doRender(IRenderer* renderer) {
        if (!renderer || !isVisible() || !isDirty()) return;
        const UITheme* theme = getEffectiveTheme();
        if (!theme) return;
        UIStyle style = *theme->getStyle(styleType_).get();
        glm::vec4 fadedColor = style.backgroundColor * glm::vec4(1.0f, 1.0f, 1.0f, opacity_);
        renderer->drawRect(position_, size_, fadedColor);
        titleLabel_->render(renderer);
        messageLabel_->render(renderer);
        for (auto& child : getMutableChildren()) {
            if (child)
                child->render(renderer);
        }
        markDirty();
    }

    void UIDialog::close() {
        setVisible(false);
        if (onClose_) onClose_();
    }

    void UIDialog::onStyleUpdate() {
        markDirty();
    }

    void UIDialog::registerEventHandlers() {
        // Implement as needed.
    }

    void UIDialog::onKeyPress(UIElement* , EventType ) {
        // Implement as needed.
    }

} // namespace ui
