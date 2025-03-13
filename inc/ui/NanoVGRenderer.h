#pragma once
#include "IRenderer.h"
#include "ITexture.h"
#include <nanovg.h>
#include <glm/glm.hpp>
#include <string>
#include <optional>

namespace ui {

    class NanoVGRenderer : public IRenderer {
    public:
        explicit NanoVGRenderer(NVGcontext* ctx);
        ~NanoVGRenderer() override = default;

        // IRenderer interface implementations
        void drawRect(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color) override;
        void drawLine(const glm::vec2& start, const glm::vec2& end, const glm::vec4& color) override;
        void drawText(const glm::vec2& position, const std::string& text, const glm::vec4& color, float fontSize) override;
        void drawTexture(const glm::vec2& position, const glm::vec2& size, ITexture* texture) override;
        glm::vec2 measureText(const std::string& text, float fontSize) override;
        void setClipRect(const glm::vec2& position, const glm::vec2& size) override;
        void resetClipRect() override;
        void* getNVGContext() override;
        glm::vec2 getScreenSize() const override;

    private:
        NVGcontext* ctx_;
        std::optional<std::pair<glm::vec2, glm::vec2>> clipRect_;
        glm::vec2 screenSize_; // Default screen size; ideally set externally
    };

} // namespace ui
