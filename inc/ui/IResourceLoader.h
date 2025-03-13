#pragma once
#include <optional>
#include <memory>
#include <filesystem>
#include "ITexture.h"
#include "IShader.h"
#include "IIcon.h"

namespace ui {

class IResourceLoader {
public:
    virtual ~IResourceLoader() = default;

    // Load a texture from a file path.
    virtual std::optional<std::shared_ptr<ITexture>> loadTexture(const std::filesystem::path& path) = 0;

    // Load a shader from a file path.
    virtual std::optional<std::shared_ptr<IShader>> loadShader(const std::filesystem::path& path) = 0;

    // Load an icon from a file path.
    virtual std::optional<std::shared_ptr<IIcon>> loadIcon(const std::filesystem::path& path) = 0;
};

} // namespace ui
