#include "ui/NanoVGRenderer.h"
#include "ui/NanoVGResourceLoader.h" // For NanoVGTexture definition
#include "ui/NanoVGTexture.h"
#include <nanovg.h>
#include <glm/gtc/type_ptr.hpp>
#include <spdlog/spdlog.h>

namespace ui {

NanoVGRenderer::NanoVGRenderer(NVGcontext* ctx)
    : ctx_(ctx), screenSize_(1280.0f, 720.0f) // Default screen size; adjust as needed
{
    if (!ctx_) {
        throw std::runtime_error("NanoVGRenderer: NVGcontext is null");
    }
}

void NanoVGRenderer::drawRect(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color) {
    nvgBeginPath(ctx_);
    nvgRect(ctx_, position.x, position.y, size.x, size.y);
    nvgFillColor(ctx_, nvgRGBAf(color.r, color.g, color.b, color.a));
    nvgFill(ctx_);
}

void NanoVGRenderer::drawLine(const glm::vec2& start, const glm::vec2& end, const glm::vec4& color) {
    nvgBeginPath(ctx_);
    nvgMoveTo(ctx_, start.x, start.y);
    nvgLineTo(ctx_, end.x, end.y);
    nvgStrokeColor(ctx_, nvgRGBAf(color.r, color.g, color.b, color.a));
    nvgStroke(ctx_);
}

void NanoVGRenderer::drawText(const glm::vec2& position, const std::string& text, const glm::vec4& color, float fontSize) {
    nvgFontSize(ctx_, fontSize);
    nvgFontFace(ctx_, "sans"); // Using default font; ideally, obtain from theme
    nvgFillColor(ctx_, nvgRGBAf(color.r, color.g, color.b, color.a));
    nvgText(ctx_, position.x, position.y, text.c_str(), nullptr);
}

void NanoVGRenderer::drawTexture(const glm::vec2& position, const glm::vec2& size, ITexture* texture) {
    if (!texture) return;
    
    // Cast to NanoVGTexture to access getPaint() method.
    auto* nvgTex = dynamic_cast<NanoVGTexture*>(texture);
    if (!nvgTex) {
        spdlog::error("NanoVGRenderer::drawTexture: Provided texture is not a NanoVGTexture");
        return;
    }
    
    // Retrieve paint for the entire texture. Optionally, you can pass a color multiplier.
    NVGpaint paint = nvgTex->getPaint();
    
    nvgBeginPath(ctx_);
    nvgRect(ctx_, position.x, position.y, size.x, size.y);
    nvgFillPaint(ctx_, paint);
    nvgFill(ctx_);
}

glm::vec2 NanoVGRenderer::measureText(const std::string& text, float fontSize) {
    nvgFontSize(ctx_, fontSize);
    nvgFontFace(ctx_, "sans");
    float bounds[4];
    nvgTextBounds(ctx_, 0, 0, text.c_str(), nullptr, bounds);
    return glm::vec2(bounds[2] - bounds[0], bounds[3] - bounds[1]);
}

void NanoVGRenderer::setClipRect(const glm::vec2& position, const glm::vec2& size) {
    nvgScissor(ctx_, position.x, position.y, size.x, size.y);
    clipRect_ = std::make_pair(position, size);
}

void NanoVGRenderer::resetClipRect() {
    nvgResetScissor(ctx_);
    clipRect_.reset();
}

void* NanoVGRenderer::getNVGContext() {
    return ctx_;
}

glm::vec2 NanoVGRenderer::getScreenSize() const {
    return screenSize_;
}

} // namespace ui
