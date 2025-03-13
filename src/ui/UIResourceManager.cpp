#include "ui/UIResourceManager.h"
#include <spdlog/spdlog.h>

namespace ui {

void UIResourceManager::setFontRenderer(std::shared_ptr<IFontRenderer> renderer) {
    std::lock_guard<std::mutex> lock(resourceMutex_);
    fontRenderer_ = std::move(renderer);
}

void UIResourceManager::setResourceLoader(std::unique_ptr<IResourceLoader> loader) {
    std::lock_guard<std::mutex> lock(resourceMutex_);
    resourceLoader_ = std::move(loader);
}

std::optional<std::shared_ptr<Font>> UIResourceManager::loadFont(const std::string& name, const std::filesystem::path& path) {
    std::lock_guard<std::mutex> lock(resourceMutex_);
    // Check if already loaded
    if (auto it = fonts_.find(name); it != fonts_.end()) {
        if (auto font = it->second.lock()) {
            return font;
        }
        fonts_.erase(it);
    }

    if (!fontRenderer_) {
        spdlog::error("ResourceManager: Font renderer not set for loading font '{}'", name);
        return std::nullopt;
    }
    auto font = fontRenderer_->loadFont(name, path);
    if (!font) {
        spdlog::error("ResourceManager: Failed to load font '{}' from '{}'", name, path.string());
        return std::nullopt;
    }
    fonts_[name] = font;
    return font;
}

std::optional<std::shared_ptr<ITexture>> UIResourceManager::loadTexture(const std::filesystem::path& path) {
    std::lock_guard<std::mutex> lock(resourceMutex_);
    std::string key = path.string();
    if (auto it = textures_.find(key); it != textures_.end()) {
        if (auto texture = it->second.lock()) {
            return texture;
        }
        textures_.erase(it);
    }
    if (!resourceLoader_) {
        spdlog::error("ResourceManager: No resource loader set for texture '{}'", key);
        return std::nullopt;
    }
    auto textureOpt = resourceLoader_->loadTexture(path);
    if (!textureOpt.has_value()) {
        spdlog::error("ResourceManager: Failed to load texture from '{}'", key);
        return std::nullopt;
    }
    textures_[key] = textureOpt.value();
    return textureOpt;
}

std::optional<std::shared_ptr<IShader>> UIResourceManager::loadShader(const std::filesystem::path& path) {
    std::lock_guard<std::mutex> lock(resourceMutex_);
    std::string key = path.string();
    if (auto it = shaders_.find(key); it != shaders_.end()) {
        if (auto shader = it->second.lock()) {
            return shader;
        }
        shaders_.erase(it);
    }
    if (!resourceLoader_) {
        spdlog::error("ResourceManager: No resource loader set for shader '{}'", key);
        return std::nullopt;
    }
    auto shaderOpt = resourceLoader_->loadShader(path);
    if (!shaderOpt.has_value()) {
        spdlog::error("ResourceManager: Failed to load shader from '{}'", key);
        return std::nullopt;
    }
    shaders_[key] = shaderOpt.value();
    return shaderOpt;
}

std::optional<std::shared_ptr<IIcon>> UIResourceManager::loadIcon(const std::filesystem::path& path) {
    std::lock_guard<std::mutex> lock(resourceMutex_);
    std::string key = path.string();
    if (auto it = icons_.find(key); it != icons_.end()) {
        if (auto icon = it->second.lock()) {
            return icon;
        }
        icons_.erase(it);
    }
    if (!resourceLoader_) {
        spdlog::error("ResourceManager: No resource loader set for icon '{}'", key);
        return std::nullopt;
    }
    auto iconOpt = resourceLoader_->loadIcon(path);
    if (!iconOpt.has_value()) {
        spdlog::error("ResourceManager: Failed to load icon from '{}'", key);
        return std::nullopt;
    }
    icons_[key] = iconOpt.value();
    return iconOpt;
}

std::optional<std::shared_ptr<Font>> UIResourceManager::getFont(const std::string& name) {
    std::lock_guard<std::mutex> lock(resourceMutex_);
    if (auto it = fonts_.find(name); it != fonts_.end()) {
        if (auto font = it->second.lock()) {
            return font;
        }
        fonts_.erase(it);
    }
    return std::nullopt;
}

std::optional<std::shared_ptr<ITexture>> UIResourceManager::getTexture(const std::string& name) {
    std::lock_guard<std::mutex> lock(resourceMutex_);
    if (auto it = textures_.find(name); it != textures_.end()) {
        if (auto texture = it->second.lock()) {
            return texture;
        }
        textures_.erase(it);
    }
    return std::nullopt;
}

std::optional<std::shared_ptr<IShader>> UIResourceManager::getShader(const std::string& name) {
    std::lock_guard<std::mutex> lock(resourceMutex_);
    if (auto it = shaders_.find(name); it != shaders_.end()) {
        if (auto shader = it->second.lock()) {
            return shader;
        }
        shaders_.erase(it);
    }
    return std::nullopt;
}

std::optional<std::shared_ptr<IIcon>> UIResourceManager::getIcon(const std::string& name) {
    std::lock_guard<std::mutex> lock(resourceMutex_);
    if (auto it = icons_.find(name); it != icons_.end()) {
        if (auto icon = it->second.lock()) {
            return icon;
        }
        icons_.erase(it);
    }
    return std::nullopt;
}

} // namespace ui
