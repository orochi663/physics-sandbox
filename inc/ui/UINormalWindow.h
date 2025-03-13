#pragma once
#include "ui/UICanvas.h"
#include <memory>
#include <string>

namespace ui {

    class UINormalWindow : public UICanvas {
    public:
        static std::unique_ptr<UINormalWindow> create(const std::string& title, const std::string& styleType = "window", int zIndex = 0);
        void render(IRenderer* renderer) override;
        bool handleInput(IMouseEvent* mouseEvent) override;

        void setTitle(const std::string& title);
        void onStyleUpdate() override;

    private:
        UINormalWindow(const std::string& title, const std::string& styleType, int zIndex);
        void bringToFront();

        std::string title_;
        std::unique_ptr<UILabel> titleLabel_;
        glm::vec2 dragStart_{ 0.0f, 0.0f };
        bool isDragging_{ false };
    };

} // namespace ui
