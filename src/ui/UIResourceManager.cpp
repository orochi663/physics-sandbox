#include "UIResourceManager.h"
#include <spdlog/spdlog.h>

namespace ui {

void UIResourceManager::setFontRenderer(std::shared_ptr<IFontRenderer> renderer) {
    std::lock_guard<std::mutex> lock(resourceMutex_);
    fontRenderer_ = std::move(renderer);
}

std::optional<std::shared_ptr<Font>> UIResourceManager::loadFont(const std::string& name, const std::string& path) {
    std::lock_guard<std::mutex> lock(resourceMutex_);

    // Check if already loaded
    if (auto it = fonts_.find(name); it != fonts_.end()) {
        if (auto font = it->second.lock()) {
            return font;
        }
        fonts_.erase(it); // Clean up expired weak_ptr
    }

    // Load new font using font renderer
    if (!fontRenderer_) {
        spdlog::error("ResourceManager: Font renderer not set for loading font '{}'", name);
        return std::nullopt;
    }

    auto font = fontRenderer_->loadFont(name, path);
    if (!font) {
        spdlog::error("ResourceManager: Failed to load font '{}' from '{}'", name, path);
        return std::nullopt;
    }

    fonts_[name] = font;
    return font;
}

std::optional<std::shared_ptr<Texture>> UIResourceManager::loadTexture(const std::string& path) {
    std::lock_guard<std::mutex> lock(resourceMutex_);

    // Check if already loaded
    if (auto it = textures_.find(path); it != textures_.end()) {
        if (auto texture = it->second.lock()) {
            return texture;
        }
        textures_.erase(it);
    }

    // TODO: Implement texture loading (deferred to graphics step)
    spdlog::warn("ResourceManager: Texture loading not implemented for '{}'", path);
    return std::nullopt;
}

std::optional<std::shared_ptr<Shader>> UIResourceManager::loadShader(const std::string& path) {
    std::lock_guard<std::mutex> lock(resourceMutex_);

    // Check if already loaded
    if (auto it = shaders_.find(path); it != shaders_.end()) {
        if (auto shader = it->second.lock()) {
            return shader;
        }
        shaders_.erase(it);
    }

    // TODO: Implement shader loading (deferred to graphics step)
    spdlog::warn("ResourceManager: Shader loading not implemented for '{}'", path);
    return std::nullopt;
}

std::optional<std::shared_ptr<Icon>> UIResourceManager::loadIcon(const std::string& path) {
    std::lock_guard<std::mutex> lock(resourceMutex_);

    // Check if already loaded
    if (auto it = icons_.find(path); it != icons_.end()) {
        if (auto icon = it->second.lock()) {
            return icon;
        }
        icons_.erase(it);
    }

    // TODO: Implement icon loading (deferred to graphics step)
    spdlog::warn("ResourceManager: Icon loading not implemented for '{}'", path);
    return std::nullopt;
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

std::optional<std::shared_ptr<Texture>> UIResourceManager::getTexture(const std::string& name) {
    std::lock_guard<std::mutex> lock(resourceMutex_);
    if (auto it = textures_.find(name); it != textures_.end()) {
        if (auto texture = it->second.lock()) {
            return texture;
        }
        textures_.erase(it);
    }
    return std::nullopt;
}

std::optional<std::shared_ptr<Shader>> UIResourceManager::getShader(const std::string& name) {
    std::lock_guard<std::mutex> lock(resourceMutex_);
    if (auto it = shaders_.find(name); it != shaders_.end()) {
        if (auto shader = it->second.lock()) {
            return shader;
        }
        shaders_.erase(it);
    }
    return std::nullopt;
}

std::optional<std::shared_ptr<Icon>> UIResourceManager::getIcon(const std::string& name) {
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