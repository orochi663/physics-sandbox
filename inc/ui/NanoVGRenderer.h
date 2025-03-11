#pragma once
#include "IRenderer.h"
#include "UITexture.h"
#include "UITheme.h"
#include <nanovg.h>
#include <memory>
#include <optional>
#include <string>

namespace ui {

class NanoVGRenderer : public IRenderer {
public:
    explicit NanoVGRenderer(NVGcontext* ctx);
    ~NanoVGRenderer() override = default;

    void drawRect(const glm::vec2& pos, const glm::vec2& size) override;
    void drawTexture(const glm::vec2& pos, const glm::vec2& size, std::shared_ptr<UITexture> texture) override;
    void drawText(const glm::vec2& pos, const std::string& text, float fontSize, float fontWeight,
                  const std::string& fontStyle, UITheme::Style::TextAlign align) override;
    void setClipRect(const glm::vec2& pos, const glm::vec2& size) override;
    void resetClipRect() override;
    NVGcontext* getNVGContext() override { return ctx_; }

    // Compute effective style with parent hierarchy
    UITheme::Style computeEffectiveStyle(const UIElement* element) const;

private:
    NVGcontext* ctx_;
    std::optional<std::pair<glm::vec2, glm::vec2>> clipRect_;
};

} // namespace ui