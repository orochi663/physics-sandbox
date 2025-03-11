#include "UIScrollbar.h"
#include "UICanvas.h"
#include <spdlog/spdlog.h>

namespace ui {

UIScrollbar::UIScrollbar(Orientation orientation, const std::string& id) : orientation_(orientation) {
    styleType_ = orientation == Orientation::Vertical ? "scrollbar-vertical" : "scrollbar-horizontal";
    setId(id.empty() ? styleType_ : id);
}

void UIScrollbar::render(IRenderer* renderer) {
    if (!renderer || !canvas_) return;

    Theme& theme = Theme::getInstance();
    Theme::Style style = theme.computeEffectiveStyle(this);

    glm::vec2 canvasPos = canvas_->getPosition();
    glm::vec2 canvasSize = canvas_->getSize();
    glm::vec2 contentSize = canvas_->getContentSize();
    glm::vec2 cumulativeOffset = canvas_->getCumulativeScrollOffset();
    glm::vec2 adjustedPos = canvasPos - cumulativeOffset;

    if (orientation_ == Orientation::Vertical) {
        float contentHeight = contentSize.y;
        float canvasHeight = canvasSize.y;
        if (contentHeight <= canvasHeight) return;

        thickness_ = style.borderWidth > 0.0f ? style.borderWidth : 20.0f;
        length_ = canvasHeight * (canvasHeight / contentHeight);
        float maxScroll = canvasHeight - length_;
        float scrollY = (canvas_->getScrollOffset().y / (contentHeight - canvasHeight)) * maxScroll;
        glm::vec2 scrollbarPos(adjustedPos.x + canvasSize.x - thickness_, adjustedPos.y + scrollY);

        if (style.backgroundTexture) {
            renderer->drawTexture(scrollbarPos, glm::vec2(thickness_, length_), style.backgroundTexture);
        } else {
            renderer->drawRect(scrollbarPos, glm::vec2(thickness_, length_));
        }
        // Apply background color if no texture
        if (!style.backgroundTexture) {
            nvgFillColor(renderer->getNVGContext(), nvgRGBA(static_cast<unsigned char>(style.backgroundColor.r * 255),
                                                           static_cast<unsigned char>(style.backgroundColor.g * 255),
                                                           static_cast<unsigned char>(style.backgroundColor.b * 255),
                                                           static_cast<unsigned char>(style.backgroundColor.a * 255)));
            nvgFill(renderer->getNVGContext());
        }
    } else {
        float contentWidth = contentSize.x;
        float canvasWidth = canvasSize.x;
        if (contentWidth <= canvasWidth) return;

        thickness_ = style.borderWidth > 0.0f ? style.borderWidth : 20.0f;
        length_ = canvasWidth * (canvasWidth / contentWidth);
        float maxScroll = canvasWidth - length_;
        float scrollX = (canvas_->getScrollOffset().x / (contentWidth - canvasWidth)) * maxScroll;
        glm::vec2 scrollbarPos(adjustedPos.x + scrollX, adjustedPos.y + canvasSize.y - thickness_);

        if (style.backgroundTexture) {
            renderer->drawTexture(scrollbarPos, glm::vec2(length_, thickness_), style.backgroundTexture);
        } else {
            renderer->drawRect(scrollbarPos, glm::vec2(length_, thickness_));
        }
        // Apply background color if no texture
        if (!style.backgroundTexture) {
            nvgFillColor(renderer->getNVGContext(), nvgRGBA(static_cast<unsigned char>(style.backgroundColor.r * 255),
                                                           static_cast<unsigned char>(style.backgroundColor.g * 255),
                                                           static_cast<unsigned char>(style.backgroundColor.b * 255),
                                                           static_cast<unsigned char>(style.backgroundColor.a * 255)));
            nvgFill(renderer->getNVGContext());
        }
    }
}

void UIScrollbar::handleInput(const SDL_Event& event) {
    if (!canvas_) return;

    glm::vec2 mousePos(event.button.x, event.button.y);
    switch (event.type) {
        case SDL_MOUSEBUTTONDOWN:
            if (hitTest(mousePos)) {
                dragging_ = true;
                dragStart_ = mousePos;
            }
            break;
        case SDL_MOUSEBUTTONUP:
            dragging_ = false;
            break;
        case SDL_MOUSEMOTION:
            if (dragging_) {
                updatePosition(event);
            }
            break;
        case SDL_MOUSEWHEEL:
            float scrollAmount = event.wheel.y * 10.0f; // Arbitrary scroll speed
            if (orientation_ == Orientation::Vertical) {
                float newY = canvas_->getScrollOffset().y - scrollAmount;
                canvas_->setScrollOffset(canvas_->getScrollOffset().x, newY);
            } else {
                float newX = canvas_->getScrollOffset().x - scrollAmount;
                canvas_->setScrollOffset(newX, canvas_->getScrollOffset().y);
            }
            break;
        case SDL_KEYDOWN:
            if (hitTest(glm::vec2(event.key.x, event.key.y))) { // Approximate position
                float scrollStep = 20.0f; // Arbitrary step size
                if (orientation_ == Orientation::Vertical) {
                    switch (event.key.keysym.sym) {
                        case SDLK_UP: canvas_->setScrollOffset(canvas_->getScrollOffset().x, canvas_->getScrollOffset().y - scrollStep); break;
                        case SDLK_DOWN: canvas_->setScrollOffset(canvas_->getScrollOffset().x, canvas_->getScrollOffset().y + scrollStep); break;
                        case SDLK_PAGEUP: canvas_->setScrollOffset(canvas_->getScrollOffset().x, canvas_->getScrollOffset().y - canvas_->getSize().y * 0.5f); break;
                        case SDLK_PAGEDOWN: canvas_->setScrollOffset(canvas_->getScrollOffset().x, canvas_->getScrollOffset().y + canvas_->getSize().y * 0.5f); break;
                    }
                } else {
                    switch (event.key.keysym.sym) {
                        case SDLK_LEFT: canvas_->setScrollOffset(canvas_->getScrollOffset().x - scrollStep, canvas_->getScrollOffset().y); break;
                        case SDLK_RIGHT: canvas_->setScrollOffset(canvas_->getScrollOffset().x + scrollStep, canvas_->getScrollOffset().y); break;
                        case SDLK_PAGEUP: canvas_->setScrollOffset(canvas_->getScrollOffset().x - canvas_->getSize().x * 0.5f, canvas_->getScrollOffset().y); break;
                        case SDLK_PAGEDOWN: canvas_->setScrollOffset(canvas_->getScrollOffset().x + canvas_->getSize().x * 0.5f, canvas_->getScrollOffset().y); break;
                    }
                }
            }
            break;
    }
}

void UIScrollbar::onStyleUpdate() {
    Theme& theme = Theme::getInstance();
    Theme::Style style = theme.computeEffectiveStyle(this);

    // Recompute thickness based on style
    thickness_ = style.borderWidth > 0.0f ? style.borderWidth : 20.0f;

    // Update appearance (e.g., ensure texture or color is applied in render)
    // This is handled in render(), so no additional action needed here
}

void UIScrollbar::attachToCanvas(UICanvas* canvas) {
    if (!canvas) {
        spdlog::error("UIScrollbar: Cannot attach to null canvas");
        return;
    }
    canvas_ = canvas;
}

bool UIScrollbar::hitTest(const glm::vec2& point) const {
    if (!canvas_) return false;

    glm::vec2 canvasPos = canvas_->getPosition();
    glm::vec2 canvasSize = canvas_->getSize();
    glm::vec2 contentSize = canvas_->getContentSize();
    glm::vec2 cumulativeOffset = canvas_->getCumulativeScrollOffset();
    glm::vec2 adjustedPos = canvasPos - cumulativeOffset;

    if (orientation_ == Orientation::Vertical) {
        float contentHeight = contentSize.y;
        float canvasHeight = canvasSize.y;
        if (contentHeight <= canvasHeight) return false;

        float maxScroll = canvasHeight - length_;
        float scrollY = (canvas_->getScrollOffset().y / (contentHeight - canvasHeight)) * maxScroll;
        float scrollbarX = adjustedPos.x + canvasSize.x - thickness_;
        float scrollbarY = adjustedPos.y + scrollY;
        return (point.x >= scrollbarX && point.x <= scrollbarX + thickness_ &&
                point.y >= scrollbarY && point.y <= scrollbarY + length_);
    } else {
        float contentWidth = contentSize.x;
        float canvasWidth = canvasSize.x;
        if (contentWidth <= canvasWidth) return false;

        float maxScroll = canvasWidth - length_;
        float scrollX = (canvas_->getScrollOffset().x / (contentWidth - canvasWidth)) * maxScroll;
        float scrollbarX = adjustedPos.x + scrollX;
        float scrollbarY = adjustedPos.y + canvasSize.y - thickness_;
        return (point.x >= scrollbarX && point.x <= scrollbarX + length_ &&
                point.y >= scrollbarY && point.y <= scrollbarY + thickness_);
    }
}

void UIScrollbar::updatePosition(const SDL_Event& event) {
    glm::vec2 mousePos(event.button.x, event.button.y);
    glm::vec2 delta = mousePos - dragStart_;
    dragStart_ = mousePos;

    if (orientation_ == Orientation::Vertical) {
        float canvasHeight = canvas_->getSize().y;
        float contentHeight = canvas_->getContentSize().y;
        float maxScroll = contentHeight - canvasHeight;
        float scrollPerPixel = maxScroll / (canvasHeight - length_);
        float newY = canvas_->getScrollOffset().y + delta.y * scrollPerPixel;
        canvas_->setScrollOffset(canvas_->getScrollOffset().x, newY);
    } else {
        float canvasWidth = canvas_->getSize().x;
        float contentWidth = canvas_->getContentSize().x;
        float maxScroll = contentWidth - canvasWidth;
        float scrollPerPixel = maxScroll / (canvasWidth - length_);
        float newX = canvas_->getScrollOffset().x + delta.x * scrollPerPixel;
        canvas_->setScrollOffset(newX, canvas_->getScrollOffset().y);
    }
}

void UIScrollbar::updateScrollOffset() {
    // Called by UICanvas if needed (e.g., after layout change)
}

} // namespace ui