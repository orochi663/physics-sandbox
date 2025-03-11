#pragma once
#include "UICanvas.h"

namespace ui {

class UIDockable : public UICanvas {
public:
    enum class DockPosition { None, Top, Bottom, Left, Right };

    static std::unique_ptr<UIDockable> create(const std::string& title, const std::string& styleType = "dockable", int zIndex = 0);
    void render(IRenderer* renderer) override;
    bool handleInput(IMouseEvent* mouseEvent) override;
    void onStyleUpdate() override;

    void setDockPosition(DockPosition position);
    DockPosition getDockPosition() const { return dockPosition_; }
    void setTitle(const std::string& title) { title_ = title; titleLabel_->setText(title); markDirty(); }

private:
    UIDockable(const std::string& title, const std::string& styleType, int zIndex);
    void updateDocking();

    std::string title_;
    std::unique_ptr<UILabel> titleLabel_;
    DockPosition dockPosition_{DockPosition::None};
    glm::vec2 dragStart_;
    bool isDragging_{false};
    float titleBarHeight_{20.0f};
};

} // namespace ui