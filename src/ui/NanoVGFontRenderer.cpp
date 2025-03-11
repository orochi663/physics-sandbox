#include "NanoVGFontRenderer.h"
#include <nanovg.h>

namespace ui {

NanoVGFont::NanoVGFont(const std::string& name, int fontId, int boldId, int italicId)
    : name_(name), fontId_(fontId), boldId_(boldId), italicId_(italicId) {}

std::string NanoVGFont::getName() const {
    return name_;
}

NanoVGFontRenderer::NanoVGFontRenderer(NVGcontext* ctx) : ctx_(ctx) {}

std::shared_ptr<IFont> NanoVGFontRenderer::loadFont(const std::string& name, const std::filesystem::path& path) {
    if (!ctx_) return nullptr;
    int fontId = nvgCreateFont(ctx_, name.c_str(), path.string().c_str());
    if (fontId == -1) return nullptr;

    // Attempt to load bold and italic variants (assuming filenames like "font-bold.ttf", "font-italic.ttf")
    std::string baseName = name.substr(0, name.find_last_of('.'));
    std::string ext = name.substr(name.find_last_of('.'));
    int boldId = -1, italicId = -1;
    std::string boldPath = baseName + "-bold" + ext;
    std::string italicPath = baseName + "-italic" + ext;
    if (nvgCreateFont(ctx_, (baseName + "-bold").c_str(), boldPath.c_str()) != -1) {
        boldId = nvgFindFont(ctx_, (baseName + "-bold").c_str());
    }
    if (nvgCreateFont(ctx_, (baseName + "-italic").c_str(), italicPath.c_str()) != -1) {
        italicId = nvgFindFont(ctx_, (baseName + "-italic").c_str());
    }

    return std::make_shared<NanoVGFont>(name, fontId, boldId, italicId);
}

void NanoVGFontRenderer::prepareFont(IRenderer* renderer, const IFont* font) {
    if (!ctx_ || !font) return;
    const auto* nvgFont = dynamic_cast<const NanoVGFont*>(font);
    if (!nvgFont) return;

    // Select font based on style (basic approximation)
    int fontId = nvgFont->getFontId();
    if (nvgFont->getBoldId() != -1 && renderer->getThemeStyle().fontWeight >= 700.0f) {
        fontId = nvgFont->getBoldId();
    } else if (nvgFont->getItalicId() != -1 && renderer->getThemeStyle().fontStyle == "italic") {
        fontId = nvgFont->getItalicId();
    }
    nvgFontFaceId(ctx_, fontId);
}

glm::vec2 NanoVGFontRenderer::getTextBounds(const IFont* font, const std::string& text) {
    if (!ctx_ || !font) return glm::vec2(0.0f);
    const auto* nvgFont = dynamic_cast<const NanoVGFont*>(font);
    if (!nvgFont) return glm::vec2(0.0f);
    nvgFontFaceId(ctx_, nvgFont->getFontId()); // Use base font for bounds
    float bounds[4];
    nvgTextBounds(ctx_, 0, 0, text.c_str(), nullptr, bounds);
    return glm::vec2(bounds[2] - bounds[0], bounds[3] - bounds[1]);
}

} // namespace ui