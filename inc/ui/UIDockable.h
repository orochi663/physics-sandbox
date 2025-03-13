#pragma once
#include "UICanvas.h"
#include "UILabel.h"
#include <memory>
#include <string>
#include <glm/glm.hpp>

namespace ui {

    class UIDockable : public UICanvas {
    public:
        enum class DockPosition { None, Top, Bottom, Left, Right };

        static std::unique_ptr<UIDockable> create(const std::string& title, const std::string& styleType = "dockable", int zIndex = 0);
        void render(IRenderer* renderer) override;
        bool handleInput(IMouseEvent* mouseEvent) override;

        void setDockPosition(DockPosition position);
        DockPosition getDockPosition() const { return dockPosition_; }
        void setTitle(const std::string& title);
        void setDragging(bool dragging) { isDragging_ = dragging; }
        bool isDragging() const { return isDragging_; }
        glm::vec2 getDragStart() const { return dragStart_; }
        virtual void setPosition(const glm::vec2& pos) override;
        void onStyleUpdate() override;

    private:
        UIDockable(const std::string& title, const std::string& styleType, int zIndex);
        void updateDocking();

        std::string title_;
        std::unique_ptr<UILabel> titleLabel_;
        DockPosition dockPosition_{ DockPosition::None };
        glm::vec2 dragStart_{ 0.0f, 0.0f };
        bool isDragging_{ false };
        float titleBarHeight_{ 20.0f };
    };

} // namespace ui
