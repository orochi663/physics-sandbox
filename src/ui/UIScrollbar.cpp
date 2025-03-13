#include "ui/UIScrollbar.h"
#include "ui/UICanvas.h"
#include "ui/IRenderer.h"
#include <spdlog/spdlog.h>
#include <glm/glm.hpp>
#include <algorithm>
#include <nanovg.h>

namespace ui {

    UIScrollbar::UIScrollbar(Orientation orientation, const std::string& id)
        : orientation_(orientation) {
        // Set an identifier based on orientation if none is provided
        setId(id.empty() ? (orientation == Orientation::Vertical ? "scrollbar-vertical" : "scrollbar-horizontal") : id);
        // Register for style update events via the event bus
        registerEventHandler("styleUpdate", [this](UIElement*, EventType) { onStyleUpdate(); });
    }

    bool UIScrollbar::handleInput(IMouseEvent* mouseEvent) {
        if (!mouseEvent || !canvas_) return false;
        glm::vec2 mousePos = mouseEvent->getPosition();

        switch (mouseEvent->getType()) {
        case EventType::MousePress:
            if (mouseEvent->getButton() == MouseButton::Left && hitTest(mousePos)) {
                dragging_ = true;
                dragStart_ = mousePos;
                return true;
            }
            break;
        case EventType::MouseRelease:
            if (dragging_) {
                dragging_ = false;
                return true;
            }
            break;
        case EventType::MouseMove:
            if (dragging_) {
                glm::vec2 delta = mousePos - dragStart_;
                dragStart_ = mousePos;
                updatePosition(delta);
                return true;
            }
            break;
        default:
            break;
        }
        return false;
    }

    bool UIScrollbar::hitTest(const glm::vec2& point) const {
        if (!canvas_) return false;
        glm::vec2 canvasPos = canvas_->getPosition();
        glm::vec2 canvasSize = canvas_->getSize();

        if (orientation_ == Orientation::Vertical) {
            float scrollbarX = canvasPos.x + canvasSize.x - thickness_;
            return (point.x >= scrollbarX && point.x <= scrollbarX + thickness_ &&
                point.y >= canvasPos.y && point.y <= canvasPos.y + canvasSize.y);
        }
        else { // Horizontal
            float scrollbarY = canvasPos.y + canvasSize.y - thickness_;
            return (point.x >= canvasPos.x && point.x <= canvasPos.x + canvasSize.x &&
                point.y >= scrollbarY && point.y <= scrollbarY + thickness_);
        }
    }

    void UIScrollbar::updatePosition(const glm::vec2& delta) {
        if (!canvas_) return;

        if (orientation_ == Orientation::Vertical) {
            float contentHeight = canvas_->getContentSize().y;
            float canvasHeight = canvas_->getSize().y;
            if (contentHeight <= canvasHeight) return;

            // Calculate thumb length based on the ratio of visible area to total content
            length_ = canvasHeight * (canvasHeight / contentHeight);
            float maxScrollThumb = canvasHeight - length_;
            // Compute proportional scroll change
            float scrollDelta = (maxScrollThumb > 0.0f) ? (delta.y / maxScrollThumb) * (contentHeight - canvasHeight) : 0.0f;
            glm::vec2 currentOffset = canvas_->getScrollOffset();
            canvas_->setScrollOffset(currentOffset.x, currentOffset.y + scrollDelta);
        }
        else {
            float contentWidth = canvas_->getContentSize().x;
            float canvasWidth = canvas_->getSize().x;
            if (contentWidth <= canvasWidth) return;

            length_ = canvasWidth * (canvasWidth / contentWidth);
            float maxScrollThumb = canvasWidth - length_;
            float scrollDelta = (maxScrollThumb > 0.0f) ? (delta.x / maxScrollThumb) * (contentWidth - canvasWidth) : 0.0f;
            glm::vec2 currentOffset = canvas_->getScrollOffset();
            canvas_->setScrollOffset(currentOffset.x + scrollDelta, currentOffset.y);
        }
    }

    void UIScrollbar::onStyleUpdate() {
        spdlog::debug("UIScrollbar::onStyleUpdate: updating style parameters");
        // Optionally update thickness or other parameters based on the current theme.
        // For now, we leave thickness_ as its default value.
    }

    void UIScrollbar::attachToCanvas(UICanvas* canvas) {
        if (!canvas) {
            spdlog::error("UIScrollbar: Cannot attach to null canvas");
            return;
        }
        canvas_ = canvas;
    }

    void UIScrollbar::render(IRenderer* renderer) {
        if (!renderer || !canvas_) return;

        glm::vec2 canvasPos = canvas_->getPosition();
        glm::vec2 canvasSize = canvas_->getSize();

        if (orientation_ == Orientation::Vertical) {
            float contentHeight = canvas_->getContentSize().y;
            if (contentHeight <= canvasSize.y) return; // No scrolling needed

            length_ = canvasSize.y * (canvasSize.y / contentHeight);
            float maxScrollContent = contentHeight - canvasSize.y;
            float maxScrollThumb = canvasSize.y - length_;
            float currentScroll = canvas_->getScrollOffset().y;
            float scrollY = (maxScrollContent > 0) ? (currentScroll / maxScrollContent) * maxScrollThumb : 0.0f;
            float scrollbarX = canvasPos.x + canvasSize.x - thickness_;

            nvgBeginPath(renderer->getNVGContext());
            nvgRect(renderer->getNVGContext(), scrollbarX, canvasPos.y + scrollY, thickness_, length_);
            nvgFillColor(renderer->getNVGContext(), nvgRGBA(150, 150, 150, 255));
            nvgFill(renderer->getNVGContext());
        }
        else { // Horizontal
            float contentWidth = canvas_->getContentSize().x;
            if (contentWidth <= canvasSize.x) return; // No scrolling needed

            length_ = canvasSize.x * (canvasSize.x / contentWidth);
            float maxScrollContent = contentWidth - canvasSize.x;
            float maxScrollThumb = canvasSize.x - length_;
            float currentScroll = canvas_->getScrollOffset().x;
            float scrollX = (maxScrollContent > 0) ? (currentScroll / maxScrollContent) * maxScrollThumb : 0.0f;
            float scrollbarY = canvasPos.y + canvasSize.y - thickness_;

            nvgBeginPath(renderer->getNVGContext());
            nvgRect(renderer->getNVGContext(), canvasPos.x + scrollX, scrollbarY, length_, thickness_);
            nvgFillColor(renderer->getNVGContext(), nvgRGBA(150, 150, 150, 255));
            nvgFill(renderer->getNVGContext());
        }
    }

} // namespace ui
