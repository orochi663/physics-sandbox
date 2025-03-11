#pragma once
#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <unordered_map>

namespace ui {

class ITexture;
class IShader;
class IFont;

class IRenderer {
public:
    virtual ~IRenderer() = default;
    virtual void drawRect(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color) = 0;
    virtual void drawLine(const glm::vec2& start, const glm::vec2& end, const glm::vec4& color) = 0;
    virtual void drawText(const glm::vec2& position, const std::string& text, const glm::vec4& color, float fontSize) = 0;
    virtual void drawTexture(const glm::vec2& position, const glm::vec2& size, ITexture* texture) = 0;
    virtual glm::vec2 measureText(const std::string& text, float fontSize) = 0;
    virtual void setClipRect(const glm::vec2& position, const glm::vec2& size) = 0;
    virtual void resetClipRect() = 0;
    virtual void* getNVGContext() = 0;
    virtual glm::vec2 getScreenSize() const = 0; // New method
};

} // namespace ui