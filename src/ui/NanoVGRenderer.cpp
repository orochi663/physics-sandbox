#include "NanoVGRenderer.h"
#include <nanovg.h>
#include <glm/gtc/type_ptr.hpp>
#include <spdlog/spdlog.h>

namespace ui {

NanoVGRenderer::NanoVGRenderer() {
    context_ = nvgCreateGL3(NVG_ANTIALIAS | NVG_STENCIL_STROKES);
    if (!context_) {
        throw std::runtime_error("Could not initialize NanoVG");
    }
}

NanoVGRenderer::~NanoVGRenderer() {
    if (context_) {
        nvgDeleteGL3(context_);
    }
}

void NanoVGRenderer::drawRect(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color) {
    nvgBeginPath(context_);
    nvgRect(context_, position.x, position.y, size.x, size.y);
    nvgFillColor(context_, nvgRGBAf(color.r, color.g, color.b, color.a));
    nvgFill(context_);
}

void NanoVGRenderer::drawLine(const glm::vec2& start, const glm::vec2& end, const glm::vec4& color) {
    nvgBeginPath(context_);
    nvgMoveTo(context_, start.x, start.y);
    nvgLineTo(context_, end.x, end.y);
    nvgStrokeColor(context_, nvgRGBAf(color.r, color.g, color.b, color.a));
    nvgStroke(context_);
}

void NanoVGRenderer::drawText(const glm::vec2& position, const std::string& text, const glm::vec4& color, float fontSize) {
    nvgBeginFrame(context_, 1280.0f, 720.0f, 1.0f); // Placeholder; should use dynamic size
    nvgFontSize(context_, fontSize);
    nvgFontFace(context_, "sans");
    nvgFillColor(context_, nvgRGBAf(color.r, color.g, color.b, color.a));
    nvgText(context_, position.x, position.y, text.c_str(), nullptr);
    nvgEndFrame(context_);
}

glm::vec2 NanoVGRenderer::measureText(const std::string& text, float fontSize) {
    nvgBeginFrame(context_, 1280.0f, 720.0f, 1.0f); // Placeholder
    nvgFontSize(context_, fontSize);
    nvgFontFace(context_, "sans");
    float bounds[4];
    nvgTextBounds(context_, 0, 0, text.c_str(), nullptr, bounds);
    nvgEndFrame(context_);
    return glm::vec2(bounds[2] - bounds[0], bounds[3] - bounds[1]);
}

void NanoVGRenderer::drawTexture(const glm::vec2& position, const glm::vec2& size, ITexture* texture) {
    if (!texture) return;
    nvgBeginPath(context_);
    nvgRect(context_, position.x, position.y, size.x, size.y);
    nvgFillPaint(context_, texture->getPaint());
    nvgFill(context_);
}

void NanoVGRenderer::setClipRect(const glm::vec2& position, const glm::vec2& size) {
    nvgScissor(context_, position.x, position.y, size.x, size.y);
}

void NanoVGRenderer::resetClipRect() {
    nvgResetScissor(context_);
}

void* NanoVGRenderer::getNVGContext() {
    return context_;
}

glm::vec2 NanoVGRenderer::getScreenSize() const {
    // Placeholder implementation; should integrate with actual window size
    return glm::vec2(1280.0f, 720.0f); // Replace with dynamic size from SDL/GLFW
}

} // namespace ui