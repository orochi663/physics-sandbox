#include "ui/NanoVGResourceLoader.h"
#include "ui/NanoVGTexture.h"
#include <spdlog/spdlog.h>
#include <nanovg.h>

namespace ui {

std::optional<std::shared_ptr<ITexture>> NanoVGResourceLoader::loadTexture(const std::filesystem::path& path) {
    if (!ctx_) {
        spdlog::error("NanoVGResourceLoader: NVGcontext is null");
        return std::nullopt;
    }
    
    // Load image using NanoVG's image loader.
    int imageId = nvgCreateImage(ctx_, path.string().c_str(), 0); // 0 flags for default behavior
    if (imageId == -1) {
        spdlog::error("NanoVGResourceLoader: Failed to load texture from {}", path.string());
        return std::nullopt;
    }
    
    int width = 0, height = 0;
    nvgImageSize(ctx_, imageId, &width, &height);
    
    // Create a NanoVGTexture instance wrapping the loaded image.
    auto texture = std::make_shared<NanoVGTexture>(path.filename().string(), imageId, width, height, ctx_);
    return texture;
}

std::optional<std::shared_ptr<IShader>> NanoVGResourceLoader::loadShader(const std::filesystem::path&) {
	
    spdlog::warn("NanoVGResourceLoader: Shader loading not supported in NanoVG; returning null.");
    return std::nullopt;
}

// A concrete implementation of IIcon for NanoVG resources.
class NanoVGIcon : public IIcon {
public:
    NanoVGIcon(const std::string& name, const glm::vec2& size, std::shared_ptr<ITexture> texture)
        : name_(name), size_(size), texture_(std::move(texture)) {}
    ~NanoVGIcon() override = default;
    
    std::string getName() const override { return name_; }
    glm::vec2 getSize() const override { return size_; }
    
    // Optionally, provide access to the underlying texture.
    std::shared_ptr<ITexture> getTexture() const { return texture_; }
    
private:
    std::string name_;
    glm::vec2 size_;
    std::shared_ptr<ITexture> texture_;
};

std::optional<std::shared_ptr<IIcon>> NanoVGResourceLoader::loadIcon(const std::filesystem::path& path) {
    // Load the image as a texture first.
    auto textureOpt = loadTexture(path);
    if (!textureOpt.has_value()) {
        spdlog::error("NanoVGResourceLoader: Failed to load icon from {}", path.string());
        return std::nullopt;
    }
    
    auto texture = textureOpt.value();
    glm::vec2 size(static_cast<float>(texture->getWidth()), static_cast<float>(texture->getHeight()));
    auto icon = std::make_shared<NanoVGIcon>(path.filename().string(), size, texture);
    return icon;
}

} // namespace ui
