#pragma once
#include "ui/UIElement.h"
#include <functional>
#include <memory>

namespace ui {

    class UILabel;

    class UISlider : public UIElement {
    public:
        static std::unique_ptr<UISlider> create(float minValue = 0.0f, float maxValue = 100.0f, float initialValue = 50.0f);
        void render(IRenderer* renderer) override;
        bool handleInput(IMouseEvent* mouseEvent) override;
        bool handleInput(IKeyboardEvent* keyboardEvent) override;

        void setValue(float value);
        float getValue() const { return value_; }
        void setOnValueChanged(std::function<void(float)> callback) { onValueChanged_ = std::move(callback); }

        void onStyleUpdate() override;

    private:
        UISlider(float minValue, float maxValue, float initialValue);
        void updateHandlePosition();

        float minValue_;
        float maxValue_;
        float value_;
        glm::vec2 handleSize_{ 10.0f, 20.0f };
        glm::vec2 handlePos_;
        bool isDragging_{ false };
        glm::vec2 dragStart_{ 0.0f, 0.0f };
        std::unique_ptr<UILabel> valueLabel_;
        std::function<void(float)> onValueChanged_;
    };

} // namespace ui
