#pragma once
#include "IRenderer.h"
#include "Theme.h"
#include "UIElement.h"
#include <glm/glm.hpp>
#include <memory>

namespace ui {

class UICanvas; // Forward declaration

class UIScrollbar : public UIElement {
public:
    enum class Orientation { Horizontal, Vertical };

    explicit UIScrollbar(Orientation orientation, const std::string& id = "");
    ~UIScrollbar() override = default;

    // UIElement overrides
    void render(IRenderer* renderer) override;
    void handleInput(const SDL_Event& event) override;
    void onStyleUpdate() override;

    // Scrollbar-specific
    void attachToCanvas(UICanvas* canvas);
    bool hitTest(const glm::vec2& point) const;

private:
    Orientation orientation_;
    UICanvas* canvas_{nullptr};
    float length_{0.0f};
    float thickness_{20.0f};
    float position_{0.0f}; // 0.0 to 1.0 (percentage of scroll)
    bool dragging_{false};
    glm::vec2 dragStart_{0.0f, 0.0f};

    // Helper methods
    void updatePosition(const SDL_Event& event);
    void updateScrollOffset();
};

} // namespace ui