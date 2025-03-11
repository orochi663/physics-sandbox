#pragma once
#include "UICanvas.h"

namespace ui {

class UINormalWindow : public UICanvas {
public:
    enum class TitleAlignment { Left, Right }; // Legacy enum, can be removed if using UIElement's TextAlignment

    static std::unique_ptr<UINormalWindow> create(const std::string& title, const std::string& styleType = "window", int zIndex = 0);
    void render(IRenderer* renderer) override;
    bool handleInput(IMouseEvent* mouseEvent) override;

    void setTitle(const std::string& title) { title_ = title; titleLabel_->setText(title); markDirty(); }

private:
    UINormalWindow(const std::string& title, const std::string& styleType, int zIndex);
    void bringToFront();

    std::string title_;
    std::unique_ptr<UILabel> titleLabel_;
    glm::vec2 dragStart_;
    bool isDragging_{false};
    float titleBarHeight_{20.0f};
};

} // namespace ui