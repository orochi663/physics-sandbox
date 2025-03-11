#pragma once
#include "IFont.h"
#include "IRenderer.h"
#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <filesystem>

namespace ui {

class IFontRenderer {
public:
    virtual ~IFontRenderer() = default;

	virtual std::shared_ptr<IFont> loadFont(const std::string& name, const std::filesystem::path& path) = 0;
    virtual void prepareFont(IRenderer* renderer, const IFont* font) = 0;
    virtual glm::vec2 getTextBounds(const IFont* font, const std::string& text) = 0;
};

} // namespace ui