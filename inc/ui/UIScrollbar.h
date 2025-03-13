#pragma once
#include "UIElement.h"
#include "IRenderer.h"
#include "IInputEvent.h" // Contains definitions for IMouseEvent, MouseButton, etc.
#include <glm/glm.hpp>
#include <string>

namespace ui {

    class UICanvas; // Forward declaration

    class UIScrollbar : public UIElement {
    public:
        enum class Orientation { Horizontal, Vertical };

        // Constructor: specify orientation and an optional identifier
        explicit UIScrollbar(Orientation orientation, const std::string& id = "");
        ~UIScrollbar() override = default;

        // Override input handling using abstract IMouseEvent
        bool handleInput(IMouseEvent* mouseEvent) override;

        // Override style update (triggered via the event bus)
        void onStyleUpdate() override;

        // Render the scrollbar using the provided renderer
        void render(IRenderer* renderer) override;

        // Attach the scrollbar to a canvas
        void attachToCanvas(UICanvas* canvas);

        // Hit test: check if the given point lies within the scrollbar's interactive area
        bool hitTest(const glm::vec2& point) const;

    private:
        Orientation orientation_;
        UICanvas* canvas_{ nullptr };
        float length_{ 0.0f };     // Length of the scrollbar thumb (calculated dynamically)
        float thickness_{ 20.0f }; // Thickness of the scrollbar (default value)
        bool dragging_{ false };
        glm::vec2 dragStart_{ 0.0f, 0.0f };

        // Update the canvas's scroll offset based on the movement delta
        void updatePosition(const glm::vec2& delta);
    };

} // namespace ui
