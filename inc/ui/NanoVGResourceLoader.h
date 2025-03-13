#pragma once
#include "IResourceLoader.h"
#include "ITexture.h"
#include "IShader.h"
#include "IIcon.h"
#include <nanovg.h>
#include <filesystem>
#include <optional>
#include <memory>
#include <string>
#include <glm/glm.hpp>

namespace ui {

// Concrete implementation of IResourceLoader for NanoVG + SDL
class NanoVGResourceLoader : public IResourceLoader {
public:
    explicit NanoVGResourceLoader(NVGcontext* ctx) : ctx_(ctx) {}
    ~NanoVGResourceLoader() override = default;

    // Load a texture using NanoVG's image loading facility.
    std::optional<std::shared_ptr<ITexture>> loadTexture(const std::filesystem::path& path) override;

    // NanoVG does not support custom shader loading externally.
    std::optional<std::shared_ptr<IShader>> loadShader(const std::filesystem::path&) override;

    // For icons, we load them similarly to textures.
    std::optional<std::shared_ptr<IIcon>> loadIcon(const std::filesystem::path& path) override;

private:
    NVGcontext* ctx_;
};

} // namespace ui
