#pragma once
#include "IFontRenderer.h"
#include <nanovg.h>

namespace ui {

class NanoVGFont : public IFont {
public:
    NanoVGFont(const std::string& name, int fontId, int boldId = -1, int italicId = -1);
    std::string getName() const override;

    int getFontId() const { return fontId_; }
    int getBoldId() const { return boldId_; }
    int getItalicId() const { return italicId_; }

private:
    std::string name_;
    int fontId_;
    int boldId_;
    int italicId_;
};

class NanoVGFontRenderer : public IFontRenderer {
public:
    explicit NanoVGFontRenderer(NVGcontext* ctx);
    ~NanoVGFontRenderer() override = default;
    std::shared_ptr<IFont> loadFont(const std::string& name, const std::filesystem::path& path) override;
    void prepareFont(IRenderer* renderer, const IFont* font) override;
    glm::vec2 getTextBounds(const IFont* font, const std::string& text) override;

private:
    NVGcontext* ctx_;
};

} // namespace ui